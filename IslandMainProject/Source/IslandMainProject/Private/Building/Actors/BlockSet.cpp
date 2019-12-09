// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Building/Actors/BlockSet.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/Scene.h"
#include "Building/Components/BuildingBlockComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "BlockSetFunctionComponent.h"
#include "Components/ActorComponent.h"
#include "Public/Building/Components/Core/BoxAttachmentCollisionComponent.h"
#include "CreatureLandPoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/Building/Library/BuildingLibrary.h"
#include "Public/Building/Actors/WorldBuildingGrid.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MeshComponent.h"

#if WITH_EDITOR

#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

#endif



FIntVector ABlockSet::GetInverseDirectionFromKey(AActor* actor)
{
	FIntVector returnvector = FIntVector(0, 0, 0);
	for (auto& value : copyrelationdata)
	{
		if (value.Value == actor)
		{
			returnvector = value.Key;
		}
	}

	returnvector.X *= -1;
	returnvector.Y *= -1;
	returnvector.Z *= -1;

	return returnvector;
}

// Sets default values
ABlockSet::ABlockSet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Main Parent
	this->MainParent = CreateDefaultSubobject<USceneComponent>(TEXT("Main Parent"));
	//this->MainParent->SetupAttachment(RootComponent);
	RootComponent = this->MainParent;

	// Mesh Parent
	this->MeshParent = CreateDefaultSubobject<USceneComponent>(TEXT("Mesh Parent"));
	this->MeshParent->SetupAttachment(this->MainParent);

	// Functional Parent
	this->FunctionalParent = CreateDefaultSubobject<USceneComponent>(TEXT("Functional Parent"));
	this->FunctionalParent->SetupAttachment(this->MainParent);

}

TSet<UMeshComponent*> ABlockSet::GetBlockSetMeshComponents() const
{
	TSet<UMeshComponent*> meshes;
	for(UMeshComponent* staticMesh : this->m_staticMeshes)
	{
		meshes.Add(staticMesh);
	}
	for (UMeshComponent* skeletalMesh : this->m_skeletalMeshes)
	{
		meshes.Add(skeletalMesh);
	}
	return meshes;
}

TArray<UBuildingBlockComponent*> ABlockSet::GetBuildingBlocks() const
{
	return this->m_blocks;
}

UBuildingBlockComponent* ABlockSet::GetRootBlockComponent() const
{
	return this->RootBlockComponent;
}

FIntVector ABlockSet::GetRootWorldIndex() const
{
	if (this->RootBlockComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Building block set has null root block"));
		return FIntVector::ZeroValue;
	}

	FBuildingIndex rootIndex;
	if (!this->RootBlockComponent->GetLocalRootIndex(rootIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Root index not configured for root block correctly in Get Root World Index."));
		return FIntVector::ZeroValue;
	}
	return rootIndex.WorldIndex;
}

FRotator ABlockSet::GetBlockSetRotation() const
{
	//UE_LOG(LogTemp, Warning, TEXT("Block Rotation: %s"), *this->MeshParent->GetComponentRotation().ToString());
	return this->m_blockSetRotation;
}

TArray<UBoxAttachmentCollisionComponent*> ABlockSet::GetBoxAttachmentCollisions() const
{
	return this->m_boxOverlapsForAttachment;
}

EBlockSetActiveStatus ABlockSet::GetBlockSetActiveStatus() const
{
	return this->m_activeStatus;
}

TSet<ABlockSet*> ABlockSet::GetBuildingBlockSetInDistance(FIntVector Distance) const
{
	AIslandMainProjectGameModeBase* GM = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	auto worldGridActor = GM->GetWorldGridActor();
	TSet<ABlockSet*> detectedBuildingSetActors;

	detectedBuildingSetActors.Empty();
	for (int i = 0; i < m_blocks.Num(); i++)
	{
		auto blockPositions = m_blocks[i]->GetIndexPositions();
		for (auto indexPosition : blockPositions)
		{
			auto detectingBuildingSets = worldGridActor->GetBuildingBlockSetInDistance(indexPosition.Value.WorldIndex, Distance);
			detectedBuildingSetActors.Append(detectingBuildingSets);

		}
	}
	return detectedBuildingSetActors;
}

void ABlockSet::MoveBuildingBlockSet(FIntVector worldIndexPosition, FVector worldPosition)
{
	// Update block index positions due to position change
	TArray<UBuildingBlockComponent*> blocks = this->m_blocks;
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		UBuildingBlockComponent* block = blocks[i];
		block->MoveIndexesRelativeToWorldIndex(worldIndexPosition);
	}

	// Update world position
	this->SetActorLocation(worldPosition);
}

void ABlockSet::RotateBuildingBlockSet(FRotator blockSetRotation)
{
	// Its fucking weird. I'm rotating indexes locally and globally, and Im rotating local to Block Set Space. Maybe I'll make this better later
	if (this->RootBlockComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Root Block null when attempting to rotate"));
		return;
	}

	FBuildingIndex rootIndex;
	if (!this->RootBlockComponent->GetLocalRootIndex(rootIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Root index not configured for root block correctly in Rotate Building Block Set."));
		return;
	}


	// Update block index positions due to rotation
	TArray<UBuildingBlockComponent*> blocks = this->m_blocks;
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		// Going from int to float, rotating, then rounding back
		UBuildingBlockComponent* block = blocks[i];
		block->RotateIndexesRelativeToRotation(rootIndex, blockSetRotation);
	}

	// Update world rotation
	this->m_blockSetRotation = UKismetMathLibrary::ComposeRotators(this->m_blockSetRotation, blockSetRotation);
	this->MeshParent->SetRelativeRotation(this->m_blockSetRotation);
}

void ABlockSet::SetMaterial(EBuildingBlockSetMaterialState state, UMaterialInterface* material)
{
	UMaterialInterface* materialToSet = nullptr;

	switch (state)
	{
	default:
		break;
	case BUILDING_BLOCK_MAT_UNPLACEABLE:
	case BUILDING_BLOCK_MAT_PLACEABLE:
	case BUILDING_BLOCK_MAT_SHOW_DELETE:
		materialToSet = material;
		break;
	case BUILDING_BLOCK_MAT_STANDARD:
		this->ResetMaterialOnAllMeshes();
		return;
	}

	if (materialToSet == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not set material due to invalid material state"));
		return;
	}

	this->SetMaterialOnAllMeshes(materialToSet);
}

void ABlockSet::EnableBlockSet()
{
	if (this->m_activeStatus == EBlockSetActiveStatus::ENABLED)
		return;

	// Get all functional components and call enable
	TArray<UBlockSetFunctionComponent*> functionalComponents;
	this->GetComponents<UBlockSetFunctionComponent>(functionalComponents, true);
	for (UBlockSetFunctionComponent* function : functionalComponents)
	{
		function->EnableBlockSetComponent();
	}

	this->m_activeStatus = EBlockSetActiveStatus::ENABLED;
}

void ABlockSet::DisableBlockSet()
{
	if (this->m_activeStatus == EBlockSetActiveStatus::DISABLED)
		return;

	// Get all functional components and call enable
	TArray<UBlockSetFunctionComponent*> functionalComponents;
	this->GetComponents<UBlockSetFunctionComponent>(functionalComponents, true);
	for (UBlockSetFunctionComponent* function : functionalComponents)
	{
		function->DisableBlockSetComponent();
	}

	this->m_activeStatus = EBlockSetActiveStatus::DISABLED;
}

void ABlockSet::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// TODO should migrate some of this code to Primary blueprint class


	// Assign Root Block and Blocks. Assumes one with Local Index of 0 , 0. Otherwise picks first position
	TArray<UActorComponent*> aBlocks = this->GetComponentsByClass(UBuildingBlockComponent::StaticClass());
	this->m_blocks = TArray<UBuildingBlockComponent*>();
	for (size_t i = 0; i < aBlocks.Num(); i++)
	{
		UBuildingBlockComponent* block = Cast<UBuildingBlockComponent>(aBlocks[i]);

		// Add to list of blocks
		this->m_blocks.Add(block);

		// Check for root block if not already assigned
		if (this->RootBlockComponent == nullptr)
		{
			this->RootBlockComponent = block;
		}
	}

	if (this->RootBlockComponent == nullptr && m_blocks.Num() > 0)
		this->RootBlockComponent = m_blocks[0];

	// For Dragging Items into the window. Auto locks to grid. On fence if this should run during build too.
#if WITH_EDITOR
	// Move Block Relative To World Grid

	// Snaps movement of the BlockSet to the Grid
	if (currentWorldEditorOnly == nullptr)
	{
		TArray<AActor*> demWorldActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldBuildingGrid::StaticClass(), demWorldActors);
		if (demWorldActors.Num() > 0)
		{
			AWorldBuildingGrid* world = Cast<AWorldBuildingGrid>(demWorldActors[0]);
			currentWorldEditorOnly = world;

		}
	}

	if (currentWorldEditorOnly)
	{
		FIntVector indexPosition = currentWorldEditorOnly->GetWorldIndexPositionFromApproximatedWorldPosition(this->GetActorLocation());
		FVector newWorldPosition = currentWorldEditorOnly->GetWorldPositionFromIndex(indexPosition);
		this->MoveBuildingBlockSet(indexPosition, newWorldPosition);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("World Grid does not exist in map for Building Block Sets. Construction"));
	}
#endif


}

void ABlockSet::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TArray<UPrimitiveComponent*> primitives;
	GetComponents<UPrimitiveComponent>(primitives);
	for (int i = 0; i < primitives.Num(); i++)
	{
		UPrimitiveComponent* primitive = primitives[i];
		primitive->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	}
	TArray<UActorComponent*> blockSetFunctionCompList = this->GetComponentsByClass(UBlockSetFunctionComponent::StaticClass());
	for (int i = 0; i < blockSetFunctionCompList.Num(); i++)
	{
		UBlockSetFunctionComponent* v = (UBlockSetFunctionComponent*)(blockSetFunctionCompList[i]);
		v->SetBuildingSetParent(this);
	}

	// IDamageable Initialization
	{
		m_CanGetDamage = CanGetDamage;
		m_DamageableMask.Add(EDamageSource::EDS_Player);
	}
}

void ABlockSet::PostLoad()
{
	Super::PostLoad();

}

// Called when the game starts or when spawned
void ABlockSet::BeginPlay()
{
	Super::BeginPlay();

	if (m_CanGetDamage)
		InitializeBlockSetHealth();

	// Assign Static Meshes
	this->m_staticMeshes = TSet<UStaticMeshComponent*>();
	TArray<UActorComponent*> aStaticMeshes = this->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (size_t i = 0; i < aStaticMeshes.Num(); i++)
	{
		UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(aStaticMeshes[i]);
		if (mesh == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Static Mesh Is Null For Block Set: %s"), *this->GetName());
			return;
		}

		// Add to list of meshes
		this->m_staticMeshes.Add(mesh);

		// Map Materials
		this->m_originalStaticMeshMaterialMap.Add(mesh, mesh->GetMaterials());
	}

	// Assign Skeletal Meshes
	this->m_skeletalMeshes = TSet<USkeletalMeshComponent*>();
	TArray<UActorComponent*> aSkeletalMeshes = this->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	for (size_t i = 0; i < aSkeletalMeshes.Num(); i++)
	{
		USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(aSkeletalMeshes[i]);
		if (mesh == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Is Null For Block Set: %s"), *this->GetName());
			return;
		}

		// Add to list of meshes
		this->m_skeletalMeshes.Add(mesh);

		// Map Materials
		this->m_originalSkeletalMeshMaterialMap.Add(mesh, mesh->GetMaterials());
	}

	// Assign boxes for
	if (this->RequireBoxOverlapCollisionCheck)
	{
		this->GetComponents<UBoxAttachmentCollisionComponent>(this->m_boxOverlapsForAttachment);
	}
}

// Called every frame
void ABlockSet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockSet::SetMaterialOnAllMeshes(UMaterialInterface* material)
{
	for (UStaticMeshComponent* staticMesh :  this->m_staticMeshes)
	{
		if (staticMesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Material could not be set due to null Static Referenced Mesh"));
			continue;
		}

		TArray<UMaterialInterface*> originalStaticMaterials = this->m_originalStaticMeshMaterialMap[staticMesh];
		for (size_t j = 0; j < originalStaticMaterials.Num(); j++)
		{
			staticMesh->SetMaterial(j, material);
		}
	}

	for (USkeletalMeshComponent* skeletalMesh:  this->m_skeletalMeshes)
	{
		if (skeletalMesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Material could not be set due to null Skeletal Referenced Mesh"));
			continue;
		}

		TArray<UMaterialInterface*> originalSkeletalMaterials = this->m_originalSkeletalMeshMaterialMap[skeletalMesh];
		for (size_t j = 0; j < originalSkeletalMaterials.Num(); j++)
		{
			skeletalMesh->SetMaterial(j, material);
		}
	}
}

void ABlockSet::ResetMaterialOnAllMeshes()
{
	for (UStaticMeshComponent* staticMesh : this->m_staticMeshes)
	{
		if (staticMesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Material could not be reset due to null Referenced Mesh"));
			continue;
		}

		TArray<UMaterialInterface*> originalStaticMaterials = this->m_originalStaticMeshMaterialMap[staticMesh];
		for (size_t j = 0; j < originalStaticMaterials.Num(); j++)
		{
			staticMesh->SetMaterial(j, originalStaticMaterials[j]);
		}
	}

	for (USkeletalMeshComponent* skeletalMesh : this->m_skeletalMeshes)
	{
		if (skeletalMesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Material could not be set due to null Skeletal Referenced Mesh"));
			continue;
		}

		TArray<UMaterialInterface*> originalSkeletalMaterials = this->m_originalSkeletalMeshMaterialMap[skeletalMesh];
		for (size_t j = 0; j < originalSkeletalMaterials.Num(); j++)
		{
			skeletalMesh->SetMaterial(j, originalSkeletalMaterials[j]);
		}
	}
}

void ABlockSet::GetDamage_Implementation(const FDamageInfo& dInfo)
{
	if (!m_GetDamageCoolDownFinish || !m_CanGetDamage || m_DamageableMask.Contains(dInfo.DamageSource)) return;

	//Basic reduce health
	int actualDmg;
	if (dInfo.Damage > m_currentHealth)
		actualDmg = m_currentHealth;
	else actualDmg = dInfo.Damage;
	m_currentHealth -= actualDmg;

	UE_LOG(LogTemp, Log, TEXT("%s is attacking by Enemy! Damage dealt: %d, Current Health: %d"), *GetName(), actualDmg, m_currentHealth);

	BPOnGetdamage(m_currentHealth / (float)m_maxHealth);
	if (m_currentHealth <= 0)
		Die();
}

void ABlockSet::Die()
{
	AIslandMainProjectGameModeBase* GM = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());

	AWorldBuildingGrid* grid = GM->GetWorldGridActor();
	TArray<FIntVector> blockWorldIndices;

	// Get all indices in this block set
	for (auto item : m_blocks)
	{
		item->GetAllBlocksWorldIndices(blockWorldIndices);
	}
	UE_LOG(LogTemp, Log, TEXT("Indices amount for %s: %d"), *GetName(), blockWorldIndices.Num());
	// Clear those indices in world grid
	grid->ClearWorldMapsByWorldIndices(blockWorldIndices);

	// Check null in World Grid Map
	if (grid->HasNullBlockComponentsInMap()) {
		UE_LOG(LogTemp, Error, TEXT("Null Block component in world grid map"));
	}
	// destroy this actor
	Destroy();
}

void ABlockSet::InitializeBlockSetHealth()
{
	int numOfSubBlocks = 0;
	// Set building health according to building block indices 
	for (auto item : m_blocks)
	{
		numOfSubBlocks += item->GetIndexPositions().Num();
	}
	m_maxHealth = 100.f * numOfSubBlocks + ExtraHealth;
	m_currentHealth = m_maxHealth;
}

bool ABlockSet::IsFullHealth()
{
	if (m_currentHealth >= m_maxHealth)
	{
		return true;
	}
	return false;
}

#if WITH_EDITOR

void ABlockSet::EditorApplyTranslation(const FVector& DeltaTranslation, bool bAltDown, bool bShiftDown,
	bool cCtrlDown)
{
	// Snaps movement of the BlockSet to the Grid
	if (currentWorldEditorOnly == nullptr)
	{
		TArray<AActor*> demWorldActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldBuildingGrid::StaticClass(), demWorldActors);
		if (demWorldActors.Num() > 0)
		{
			AWorldBuildingGrid* world = Cast<AWorldBuildingGrid>(demWorldActors[0]);
			currentWorldEditorOnly = world;

		}
	}

	if (currentWorldEditorOnly)
	{
		FIntVector indexPosition = currentWorldEditorOnly->GetWorldIndexPositionFromApproximatedWorldPosition(this->GetActorLocation());
		FIntVector modification = FIntVector(FMath::Clamp(static_cast<int>(DeltaTranslation.X), -1, 1), FMath::Clamp(static_cast<int>(DeltaTranslation.Y), -1, 1), FMath::Clamp(static_cast<int>(DeltaTranslation.Z), -1, 1));
		FIntVector newIndexPosition = indexPosition + modification;
		FVector newWorldPosition = currentWorldEditorOnly->GetWorldPositionFromIndex(newIndexPosition);

		this->MoveBuildingBlockSet(newIndexPosition, newWorldPosition);
	}
	else
	{
		Super::EditorApplyTranslation(DeltaTranslation, bAltDown, bShiftDown, cCtrlDown);
		UE_LOG(LogTemp, Error, TEXT("World Grid does not exist in map for Building Block Sets. Editor Translation"));
	}
}

void ABlockSet::EditorApplyRotation(const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown,
	bool cCtrlDown)
{
	// Snaps movement of the BlockSet to the Grid
	if (currentWorldEditorOnly == nullptr)
	{
		TArray<AActor*> demWorldActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldBuildingGrid::StaticClass(), demWorldActors);
		if (demWorldActors.Num() > 0)
		{
			AWorldBuildingGrid* world = Cast<AWorldBuildingGrid>(demWorldActors[0]);
			currentWorldEditorOnly = world;

		}
	}

	if (currentWorldEditorOnly)
	{
		FRotator currentRotation = this->GetActorRotation();

		FRotator modification = FRotator(FMath::Clamp(DeltaRotation.Pitch, -1.0f, 1.0f) * 90.0f, FMath::Clamp(DeltaRotation.Yaw, -1.0f, 1.0f) * 90.0f, FMath::Clamp(DeltaRotation.Roll, -1.0f, 1.0f) * 90.0f);

		this->RotateBuildingBlockSet(modification);
	}
	else
	{
		Super::EditorApplyRotation(DeltaRotation, bAltDown, bShiftDown, cCtrlDown);
		UE_LOG(LogTemp, Error, TEXT("World Grid does not exist in map for Building Block Sets. Editor Rotation"));
	}
}




#endif