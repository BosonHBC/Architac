// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Building/Actors/WorldBuildingGrid.h"
#include "Public/Building/Actors/BlockSet.h"
#include "Building/Components/BuildingBlockComponent.h"
#include "Building/Components/Core/BoxAttachmentCollisionComponent.h"
#include "GameFramework/Actor.h"
#include "Public/InventoryComponent.h"
#include "IslandMainProjectGameModeBase.h"
#include "Libary/StaticFunctionLibary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "VenturePawn.h"
#include "BuildingLibrary.h"

const float AWorldBuildingGrid::blockSize = 100.0f;

// Sets default values
AWorldBuildingGrid::AWorldBuildingGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* mainParent = CreateDefaultSubobject<USceneComponent>(TEXT("Main Parent"));
	mainParent->SetupAttachment(RootComponent);

	this->BlockSetParent = CreateDefaultSubobject<USceneComponent>(TEXT("Block Set Parent"));
	this->BlockSetParent->SetupAttachment(mainParent);

	this->m_worldMap = TMap<FIntVector, UBuildingBlockComponent*>();

	
	
	this->WorldInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("World Inventory"));
}

UInventoryComponent* AWorldBuildingGrid::GetBuildingInventoryCompoonent() const
{
	return this->WorldInventory;
}

bool AWorldBuildingGrid::AttachBlockSet(ABlockSet * blockSet, bool ignoreAttachmentRequirements)
{
	if (!this->CanAttachBlockSet(blockSet, ignoreAttachmentRequirements))
	{
		return false;
	}

	// Log world block components into array
	TArray<UBuildingBlockComponent*> blocks = blockSet->GetBuildingBlocks();
	for (size_t i = 0; i < blocks.Num(); i++)
	{
		// Do not map indexes that are overlap based. They really never exist in the world
		if (blocks[i]->BlockType == EBlockType::OVERLAP)
			continue;

		// Map each index in the block component to the world map
		auto blockIndexes = blocks[i]->GetIndexPositions();
		for (auto& element : blockIndexes)
		{
			this->m_worldMap.Add(element.Value.WorldIndex, blocks[i]);
		}
	}

	// Enable Collision
	blockSet->SetActorEnableCollision(true);

	// Set Parent
	blockSet->AttachToComponent(this->BlockSetParent, FAttachmentTransformRules::KeepWorldTransform);

	return true;
}

bool AWorldBuildingGrid::DestroyBlockSet(FIntVector index)
{
	if(!this->m_worldMap.Contains(index))
		return false;

	ABlockSet* blockSetToDelete = this->m_worldMap[index]->GetBlockSetParent();
	TArray<UBuildingBlockComponent*> blockCoomponents = blockSetToDelete->GetBuildingBlocks();
	for (UBuildingBlockComponent* block : blockCoomponents)
	{
		auto blockIndexes = block->GetIndexPositions();
		for (auto& element : blockIndexes)
		{
			this->m_worldMap.Remove(element.Value.WorldIndex);
		}
	}
	blockSetToDelete->Destroy();
	return true;
	
}

bool AWorldBuildingGrid::CanAttachBlockSet(ABlockSet* blockSet, bool ignoreAttachmentRequirements) const
{
	if (blockSet == nullptr)
		return false;

	// Check collision with configurations. Originally made for checking collision against player so that the player can't build on top of themselves
	TSet<UMeshComponent*> meshes = blockSet->GetBlockSetMeshComponents();
	TArray<AActor*> actorFilter;
	actorFilter.Add(blockSet);
	// // Per Class Filter
	for(UClass* classFilter : this->BlockSetAttachmentClassCollisionFilter)
	{
		// Per Mesh component on the block set
		for (UMeshComponent* mesh : meshes)
		{
			TArray<AActor*> overlappingActors;

			UKismetSystemLibrary::ComponentOverlapActors(mesh, mesh->GetComponentToWorld(), this->BlockSetAttachmentObjectTypeCollisionFilter, classFilter, actorFilter, overlappingActors);

			// For Testing Collisions
			/*UE_LOG(LogTemp, Warning, TEXT("Overlapping Actors Found: %i"), overlappingActors.Num())
				for (AActor* overlap : overlappingActors)
				{
					UE_LOG(LogTemp, Warning, TEXT("----- %s"), *overlap->GetName())
				}*/
			if (overlappingActors.Num() > 0)
				return false;
		}
	}

	// For each block component, check constraints
	TArray<UBuildingBlockComponent*> blockComponents = blockSet->GetBuildingBlocks();

	// Conditions to track properties of processing this block set
	bool attachableAdjacentIndexByAtLeastOneRequirement = false;
	bool overlappedByAtLeastOneRequirement = false;
	
	// Go through each block component and calculate requirements
	for (size_t i = 0; i < blockComponents.Num(); i++)
	{
		// Check if the block component is in boundaries. Any block component not in boundaries means the block set cannot be attached currently
		// Each block component is composed of multiple indexes, check each index.
		auto blockIndexes = blockComponents[i]->GetIndexPositions();
		for (auto& index : blockIndexes)
		{
			FIntVector blockIndex = index.Value.WorldIndex;

			// If index is in boundaries
			if (!this->IsInBoundaries(blockIndex))
				return false;

			switch(blockComponents[i]->BlockType)
			{
				case EBlockType::ATTACHABLE: // Means things can be built next to it, and it takes up all 6 sides of space
					// If this index is already occupied not attachable
					if (this->IsOccupied(blockIndex))
						return false;

					// Attachment requirements
					if (!ignoreAttachmentRequirements)
					{
						// Check if the block component can be attached to another block component
						if (!attachableAdjacentIndexByAtLeastOneRequirement)
							attachableAdjacentIndexByAtLeastOneRequirement = this->IsAttachableAdjacent(blockIndex);
					}
					break;
				case EBlockType::NOT_ATTACHABLE: // // Means things can't attach to it, and it generically takes up the space of the tile
					// If this index is already occupied not attachable
					if (this->IsOccupied(blockIndex))
						return false;
					break;
				case EBlockType::OVERLAP: // Means it is just used to detect things that could take up space, this does not take up space
					if (!ignoreAttachmentRequirements)
					{
						if (!overlappedByAtLeastOneRequirement && this->IsOccupied(blockIndex) && 
							(this->m_worldMap[blockIndex]->BlockType == EBlockType::ATTACHABLE))
							overlappedByAtLeastOneRequirement = true;
					}
					break;
				default: ;
			}
		}
	}

	// Exists at end because I still want to enforce Occupation requirements
	if (ignoreAttachmentRequirements || (!blockSet->RequireAttachment && !blockSet->RequireOverlap))
		return true;

	bool canAttach = false;

	// Checking attachment via adjacency index
	if (!canAttach && blockSet->RequireAttachment)
		canAttach = attachableAdjacentIndexByAtLeastOneRequirement;

	// Checking attachment via overlap boxes
	if (!canAttach && blockSet->RequireOverlap)
		canAttach = overlappedByAtLeastOneRequirement;

	// Checking attachment via box overlap collision checks (two boxes must overlap from two different block sets to trigger true)
	if(!canAttach && blockSet->RequireBoxOverlapCollisionCheck && blockSet->BoxOverlapObjectTypes.Num() > 0)
	{
		TArray<UBoxAttachmentCollisionComponent*> boxes =  blockSet->GetBoxAttachmentCollisions();
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeFilter = blockSet->BoxOverlapObjectTypes;
		TArray<AActor*> actorFilter = TArray<AActor*>();
		actorFilter.Add(blockSet);
		TArray<UPrimitiveComponent*> outComponents;
		for(UBoxAttachmentCollisionComponent* box : boxes)
		{
			const FTransform componentTransform = box->K2_GetComponentToWorld();
			
			if(UKismetSystemLibrary::ComponentOverlapComponents(box, componentTransform, objectTypeFilter, UBoxAttachmentCollisionComponent::StaticClass(), actorFilter, outComponents))
			{
				canAttach = true;
				break;
			}

		}
	}

	// Allow custom blueprint implementation of checking
	if(!canAttach && blockSet->RequireCustomBlueprintCheck)
	{
		canAttach = blockSet->CustomBlueprintAttachCheck();
	}

	return canAttach;
}

bool AWorldBuildingGrid::IsOccupied(FIntVector index) const
{
	return this->m_worldMap.Contains(index);
}

bool AWorldBuildingGrid::IsInBoundaries(FIntVector index) const
{
	return (index.X <= (this->MiddleBlockDimensions.X + this->EdgeBlockSize) &&
		index.X >= (this->MiddleBlockDimensions.X + this->EdgeBlockSize) * -1.0f &&
		index.Y <= (this->MiddleBlockDimensions.Y + this->EdgeBlockSize) &&
		index.Y >= (this->MiddleBlockDimensions.Y + this->EdgeBlockSize) * -1.0f &&
		index.Z <= (this->MiddleBlockDimensions.Z + this->EdgeBlockSize) &&
		index.Z >= (this->MiddleBlockDimensions.Z + this->EdgeBlockSize) * -1.0f);
}

bool AWorldBuildingGrid::IsOutsideMiddle(FIntVector index) const
{
	return !(index.X <= (this->MiddleBlockDimensions.X) &&
		index.X >= (this->MiddleBlockDimensions.X) * -1.0f &&
		index.Y <= (this->MiddleBlockDimensions.Y) &&
		index.Y >= (this->MiddleBlockDimensions.Y) * -1.0f &&
		index.Z <= (this->MiddleBlockDimensions.Z) &&
		index.Z >= (this->MiddleBlockDimensions.Z) * -1.0f);
}

bool AWorldBuildingGrid::IsOnEdgeZone(FIntVector index) const
{
	return IsOutsideMiddle(index) && IsInBoundaries(index);
}

bool AWorldBuildingGrid::IsAttachableAdjacent(FIntVector index) const
{
	// Check 6 directions
	return  ((this->IsIndexAttachable(index + FIntVector(1, 0, 0))) ||
		(this->IsIndexAttachable(index + FIntVector(-1, 0, 0))) ||
		(this->IsIndexAttachable(index + FIntVector(0, 1, 0))) ||
		(this->IsIndexAttachable(index + FIntVector(0, -1, 0))) ||
		(this->IsIndexAttachable(index + FIntVector(0, 0, 1))) ||
		(this->IsIndexAttachable(index + FIntVector(0, 0, -1))));
}

FVector AWorldBuildingGrid::GetWorldPositionFromIndex(FIntVector worldIndexPosition) const
{
	FVector indexConverted = FVector(worldIndexPosition.X, worldIndexPosition.Y, worldIndexPosition.Z);
	FVector worldGridPosition = GetActorLocation();
	return worldGridPosition + this->GetActorRotation().RotateVector(
		((indexConverted * this->blockSize) + this->blockCenterOffset) * this->blockScale
	);
}

ABlockSet* AWorldBuildingGrid::GetBlockSetFromIndex(FIntVector worldIndexPosition) const
{
	if(this->m_worldMap.Contains(worldIndexPosition))
		return this->m_worldMap[worldIndexPosition]->GetBlockSetParent();
	else
		return nullptr;
}

FVector AWorldBuildingGrid::GetWorldPositionFromFloor(int floor) const
{
	return GetWorldPositionFromIndex(FIntVector(0, 0, floor));
}

FIntVector AWorldBuildingGrid::GetWorldIndexPositionFromApproximatedWorldPosition(FVector worldPosition)
{
	FVector worldGridPosition = GetActorLocation();

	FVector approxWorldGridIndexPosition = ((this->GetActorRotation().GetInverse().RotateVector(worldPosition - worldGridPosition)/this->blockScale) - this->blockCenterOffset)/ this->blockSize;
	FIntVector indexPosition = FIntVector(UKismetMathLibrary::Round(approxWorldGridIndexPosition.X), UKismetMathLibrary::Round(approxWorldGridIndexPosition.Y), UKismetMathLibrary::Round(approxWorldGridIndexPosition.Z));
	return indexPosition;
}

FIntVector AWorldBuildingGrid::GetWorldGridIndexSpawn(bool positiveZ) const
{
	FIntVector startPosition = FIntVector(0, 0, 0);

	FIntVector offset = FIntVector(0, 0, 1);
	if (!positiveZ)
		offset *= -1;

	while (true)
	{
		if (!this->IsOccupied(startPosition))
			return startPosition;

		if (!this->IsInBoundaries(startPosition + offset))
			break;

		startPosition += offset;
	}

	return startPosition;
}

bool AWorldBuildingGrid::LoadPreExistingBlockSets()
{
	if(this->FindAllBlockSetsInMap)
	{
		TArray<AActor*> blockSetActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlockSet::StaticClass(), blockSetActors);
		this->PreExistingBlockSets.Empty();
		for(AActor* actor : blockSetActors)
		{
			ABlockSet* blockSetActor = Cast<ABlockSet>(actor);
			this->PreExistingBlockSets.Add(blockSetActor);
		}
	}


	
	for(ABlockSet* blockSet : this->PreExistingBlockSets)
	{
		if(blockSet == nullptr)
			continue;

		// From Block Set, calculate the piece's approximated index position
		FIntVector approxIndexPosition = this->GetWorldIndexPositionFromApproximatedWorldPosition(blockSet->GetActorLocation());

		// Move Block Set to the world position of the approximated index position,
		FVector adjustedWorldPosition = this->GetWorldPositionFromIndex(approxIndexPosition);
		blockSet->MoveBuildingBlockSet(approxIndexPosition, adjustedWorldPosition);
		
		// Attach Block Set to world
		bool success =  this->AttachBlockSet(blockSet, true);
		if (success)
		{
			blockSet->EnableBlockSet();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed To Attach PreExisting Block Set: %s"), *blockSet->GetName());
			blockSet->Destroy();
		}
	}
	
	return true;
}

void AWorldBuildingGrid::ClearWorldMapsByWorldIndices(const TArray<FIntVector>& i_worldIndices)
{
	for (auto item : i_worldIndices)
	{
		if (!m_worldMap.FindAndRemoveChecked(item)) {
			UE_LOG(LogTemp, Error, TEXT("The world map doesn't have this index!"));
		}
	}
}

bool AWorldBuildingGrid::HasNullBlockComponentsInMap() const
{
	for (auto It = m_worldMap.CreateConstIterator(); It; ++It)
	{
		if (!UStaticFunctionLibary::IsValid(It->Value)) return true;
	}
	return false;
}

void AWorldBuildingGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AWorldBuildingGrid::BeginPlay()
{
	Super::BeginPlay();

	this->Initialize();
}

void AWorldBuildingGrid::Initialize()
{
	// Auto Load Blocksets in the world
	this->LoadPreExistingBlockSets();
}

// Called every frame
void AWorldBuildingGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TSet<ABlockSet*> AWorldBuildingGrid::GetBuildingBlockSetInDistance(FIntVector worldIndexPosition, FIntVector Distance)
{
	TSet<ABlockSet*> detectedBuildingSets;
	detectedBuildingSets.Empty();
	for (int i = -Distance.X; i <= Distance.X; i++)
	{
		for (int j = -Distance.Y; j <= Distance.Y; j++)
		{
			for (int k = -Distance.Z; k <= Distance.Z; k++)
			{
				auto detectingBuildingSet = GetBlockSetFromIndex(worldIndexPosition + FIntVector(i, j, k));
				if (detectingBuildingSet != nullptr)
				{
					detectedBuildingSets.Add(detectingBuildingSet);
				}
			}
		}
	}

	return detectedBuildingSets;
}
