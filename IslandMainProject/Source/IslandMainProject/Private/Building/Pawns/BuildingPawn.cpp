// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Building/Pawns/BuildingPawn.h"
#include "Public/Building/Actors/WorldBuildingGrid.h"
#include "Public/Building/Actors/BlockSet.h"
#include "Public/Building/Components/BuildingBlockComponent.h"
#include "Public/Building/Components/Core/BuildingTechTreeComponent.h"
#include "Public/Building/Library/BuildingLibrary.h"
#include "Engine/Classes/GameFramework/Pawn.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Public/CameraControlComponent.h"
#include "GameFramework/PlayerController.h"
#include "Public/Constants/GameInputConstants.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Public/VenturePawn.h"
#include "Public/InventoryComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
ABuildingPawn::ABuildingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OriginPoint"));
	this->RootSceneComponent->SetupAttachment(RootComponent);
	this->SetRootComponent(this->RootSceneComponent);
	
	// Primary Camera Boom
	this->PrimaryCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("PrimaryCameraBoom"));
	this->PrimaryCameraBoom->SetupAttachment(RootComponent);
	
	// Primary Camera
	this->PrimaryCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PrimaryCameraComponent"));
	this->PrimaryCameraComponent->SetupAttachment(this->PrimaryCameraBoom);

	// Camera Control Comp
    this->CameraControlComp = CreateDefaultSubobject<UCameraControlComponent>(TEXT("CameraControlComp"));

	this->m_buildingPawnState = NOT_SHOWN;

}

// Called when the game starts or when spawned
void ABuildingPawn::BeginPlay()
{
	Super::BeginPlay();
	if (this->m_currentBlockSet == nullptr && this->m_targetWorld != nullptr)
		SetTargetWorldAndInitialize(this->m_targetWorld);
}

// Called every frame
void ABuildingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//this->PrimaryCameraBoom->RelativeLocation = FVector::ZeroVector;
}

// Called to bind functionality to input
void ABuildingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Build Primary Actions
	PlayerInputComponent->BindAction(GameInputConstants::CONFIRM, IE_Pressed, this, &ABuildingPawn::ConfirmPressed);

	// BlockSet Manipulation Action Inputs
	PlayerInputComponent->BindAxis(GameInputConstants::X_MOVE_AXIS, this, &ABuildingPawn::MoveXAxis);
	PlayerInputComponent->BindAxis(GameInputConstants::Y_MOVE_AXIS, this, &ABuildingPawn::MoveYAxis);
	
	PlayerInputComponent->BindAction(GameInputConstants::MOVE_DESCEND, IE_Pressed, this, &ABuildingPawn::MoveBlockSetZNeg);
	PlayerInputComponent->BindAction(GameInputConstants::MOVE_ASCEND, IE_Pressed, this, &ABuildingPawn::MoveBlockSetZPos);

	PlayerInputComponent->BindAxis(GameInputConstants::ROTATE_MOVE_AXIS, this, &ABuildingPawn::RotateBlockSetZAxis);

	PlayerInputComponent->BindAction(GameInputConstants::CYCLE_BLOCKSET_LEFT, IE_Pressed, this, &ABuildingPawn::CycleBlockSetLeft);
	PlayerInputComponent->BindAction(GameInputConstants::CYCLE_BLOCKSET_RIGHT, IE_Pressed, this, &ABuildingPawn::CycleBlockSetRight);

	// Mode Actions
	PlayerInputComponent->BindAction(GameInputConstants::BUILD_TOGGLE_MODE, IE_Pressed, this, &ABuildingPawn::ToggleMode);
	PlayerInputComponent->BindAction(GameInputConstants::BUILD_GOTO_OBSERVE, IE_Pressed, this, &ABuildingPawn::GoToObserveMode);
	PlayerInputComponent->BindAction(GameInputConstants::BUILD_GOTO_SELECT, IE_Pressed, this, &ABuildingPawn::GoToSelectMode);
	PlayerInputComponent->BindAction(GameInputConstants::BUILD_GOTO_PLACE, IE_Pressed, this, &ABuildingPawn::GoToPlacementMode);
	PlayerInputComponent->BindAction(GameInputConstants::BUILD_GOTO_DELETE, IE_Pressed, this, &ABuildingPawn::GoToDeleteMode);

	CameraControlComp->InitializeCameraComponent();
}

void ABuildingPawn::SetTargetWorldAndInitialize(AWorldBuildingGrid* targetWorldGrid)
{
	if (targetWorldGrid == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant initialize with null target building"));
		return;
	}
	
	this->m_targetWorld = targetWorldGrid;
	
	// Set first block set to show
	if (!IsValid(m_gameMode))
		m_gameMode = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	
	TArray<FBlockSetData> possibleBlockSets = m_gameMode->GetBuildingTechTree()->GetPossibleBlockSets();
	
	if(possibleBlockSets.Num() > 0)
		this->m_currentBlockSetClass = possibleBlockSets[0].BlockSetBlueprintClass;
	else
		UE_LOG(LogTemp, Error,TEXT("Possible Block Set List Is Empty"));
}

void ABuildingPawn::EnterBuildingMode()
{
	if (this->m_currentBlockSet != nullptr)
	{
		this->ShowCurrentBlockSet();
		this->UpdatePlacementIndicator();
	}
	else
	{
		this->LoadBlockSetMap(); // Reload map everytime you're entering building mode
		this->LoadBlockSet(this->m_currentBlockSetClass, this->m_targetWorld->GetWorldGridIndexSpawn(true), FRotator::ZeroRotator);
	}

	if (this->m_lastBuildingState != NOT_SHOWN)
		this->GoToMode(this->m_lastBuildingState);
	else
		this->GoToSelectMode();
	
}

void ABuildingPawn::ExitBuildingMode()
{
	if (this->m_currentBlockSet != nullptr)
	{
		this->HideCurrentBlockSet();
		this->m_currentBlockSet->SetActorTickEnabled(false); // TODO depending on how we should optimize and handle piece function later. Look at this
		this->m_lastBuildingState = m_buildingPawnState;
		this->GoToMode(NOT_SHOWN);
	}
}

void ABuildingPawn::UIOnLoadSelectedBlockSet(TSubclassOf<ABlockSet> selectedBlockSetClass)
{
	if (!IsValid(selectedBlockSetClass))
		return;

	if (this->m_buildingPawnState != SELECTING_BLOCKSET)
		return;
	
	FIntVector currentWorldIndex = this->m_currentBlockSet->GetRootWorldIndex();
	FRotator currentRotation = this->m_currentBlockSet->GetBlockSetRotation();
	this->m_currentBlockSet->Destroy();
	this->m_currentBlockSet = nullptr;

	this->m_currentBlockSetClass = selectedBlockSetClass;
	this->LoadBlockSet(this->m_currentBlockSetClass, currentWorldIndex, currentRotation);
}

bool ABuildingPawn::ConfirmResourceRequirements(TSubclassOf<ABlockSet> selectedBlockSetClass)
{
	// Validate valid key and it is contained in the map
	if (!IsValid(selectedBlockSetClass) || !this->m_blockSetOptionsMap.Contains(selectedBlockSetClass))
		return false;

	// Validate game mode, venture pawn, and inventory
	if (!IsValid(m_gameMode))
		return false;

	// Get player inventory
	AVenturePawn* venturePawn = m_gameMode->GetVenturePawn();
	UInventoryComponent* inventoryComponent = venturePawn->GetInventoryComponent();
	
	// Get Cost
	FBlockSetData data = this->m_blockSetOptionsMap[selectedBlockSetClass];
	FItemAmountGroup* costRecord = data.CostRecord.GetRow<FItemAmountGroup>("");

	return (inventoryComponent->HasEnoughItems(*costRecord));
}

bool ABuildingPawn::SpendResourceRequirements(TSubclassOf<ABlockSet> selectedBlockSetClass)
{
	// Validate valid key and it is contained in the map
	if (!IsValid(selectedBlockSetClass) || !this->m_blockSetOptionsMap.Contains(selectedBlockSetClass))
		return false;

	// Validate game mode, venture pawn, and inventory
	if (!IsValid(m_gameMode))
		return false;

	// Get player inventory
	AVenturePawn* venturePawn = m_gameMode->GetVenturePawn();
	UInventoryComponent* inventoryComponent = venturePawn->GetInventoryComponent();

	// Get Cost
	FBlockSetData data = this->m_blockSetOptionsMap[selectedBlockSetClass];
	FItemAmountGroup* costRecord = data.CostRecord.GetRow<FItemAmountGroup>("");

	return inventoryComponent->RemoveFromInventory(*costRecord);
}

bool ABuildingPawn::RefundResourceRequirements(TSubclassOf<ABlockSet> selectedBlockSetClass)
{
	// Validate valid key and it is contained in the map
	if (!IsValid(selectedBlockSetClass) || !this->m_blockSetOptionsMap.Contains(selectedBlockSetClass))
		return false;

	// Validate game mode, venture pawn, and inventory
	if (!IsValid(m_gameMode))
		return false;

	// Get player inventory
	AVenturePawn* venturePawn = m_gameMode->GetVenturePawn();
	UInventoryComponent* inventoryComponent = venturePawn->GetInventoryComponent();

	// Get Cost
	FBlockSetData data = this->m_blockSetOptionsMap[selectedBlockSetClass];
	FItemAmountGroup* costRecord = data.CostRecord.GetRow<FItemAmountGroup>("");

	return inventoryComponent->AddToInventory(*costRecord, this->m_refundRatio);
}

#pragma region Mode Functions

void ABuildingPawn::GoToMode(EBuildingPawnState newPawnState)
{
	// Going From
	switch(this->m_buildingPawnState.GetValue())
	{
		case NOT_SHOWN: 
		break;
		case SELECTING_BLOCKSET:
		break;
		case OBSERVING_BLOCKSET:
		break;
		case PLACING_BLOCKSET:
		break;
		case DELETE_BLOCKSET:
			ShowCurrentBlockSet();
			DisableDeleteIndicator();
			this->SetBlockSetAtIndexToMaterial(this->m_currentDeleteIndicatorIndex, BUILDING_BLOCK_MAT_STANDARD);
			break;
	default: ;
	}
	
	// Going To
	switch (newPawnState)
	{
	case NOT_SHOWN:
		HideCurrentBlockSet();
		UIHideSelectionMenu();
		break;
	case SELECTING_BLOCKSET:
		{
		// Get player inventory
		AVenturePawn* venturePawn = m_gameMode->GetVenturePawn();
		UInventoryComponent* inventoryComponent = venturePawn->GetInventoryComponent();
		UILoadBlockSetOptions(m_gameMode->GetBuildingTechTree()->GetPossibleBlockSets(), inventoryComponent);
		UIShowSelectionMenu();
		break;
		}
	case OBSERVING_BLOCKSET:
		UIHideSelectionMenu();
		break;
	case PLACING_BLOCKSET:
		UIHideSelectionMenu();
		break;
	case DELETE_BLOCKSET:
		HideCurrentBlockSet();
		EnableDeleteIndicator();
		UIHideSelectionMenu();
		if (IsValid(this->m_currentBlockSet))
			MoveDeleteIndicator(this->m_currentBlockSet->GetRootWorldIndex());
		break;
	default:
		break;
	}

	this->m_buildingPawnState = newPawnState;
}

void ABuildingPawn::ToggleMode()
{
	if (this->m_buildingPawnState != SELECTING_BLOCKSET)
		this->GoToMode(SELECTING_BLOCKSET);
	else
		this->GoToMode(PLACING_BLOCKSET);
}

void ABuildingPawn::GoToSelectMode()
{
	if (this->m_buildingPawnState != SELECTING_BLOCKSET)
		this->GoToMode(SELECTING_BLOCKSET);
}

void ABuildingPawn::GoToObserveMode()
{
	if (this->m_buildingPawnState != OBSERVING_BLOCKSET)
		this->GoToMode(OBSERVING_BLOCKSET);
}

void ABuildingPawn::GoToPlacementMode()
{
	if (this->m_buildingPawnState != PLACING_BLOCKSET)
		this->GoToMode(PLACING_BLOCKSET);
}

void ABuildingPawn::GoToDeleteMode()
{
	if (this->m_buildingPawnState != DELETE_BLOCKSET)
		this->GoToMode(DELETE_BLOCKSET);
}

#pragma endregion

#pragma region Block Set Manipulation

void ABuildingPawn::LoadBlockSet(TSubclassOf<ABlockSet> blockSet, FIntVector spawnBuildingIndex, FRotator spawnRotation)
{
	if(!IsValid(blockSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("Block Set Input Invalid"));
		return;
	}
	
	// Set initial piece
	if (!m_blockSetOptionsMap.Contains(blockSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("Block Set Map does not contain BlockSet: %s"), *blockSet.Get()->GetName());
		return;
	}

	if (!IsValid(m_blockSetOptionsMap[blockSet].BlockSetBlueprintClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Block Set Does Not Contain Reference Blueprint: %s"), *blockSet->GetName());
		return;
	}

	this->CreateCurrentBlockSet(blockSet, spawnBuildingIndex, spawnRotation);
}

void ABuildingPawn::LoadBlockSetMap()
{
	if(!m_gameMode)
		m_gameMode = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());

	TArray<FBlockSetData> blockSets =  m_gameMode->GetBuildingTechTree()->GetPossibleBlockSets();

	for(size_t i = 0; i < blockSets.Num(); i++)
	{
		m_blockSetOptionsMap.Add(blockSets[i].BlockSetBlueprintClass, blockSets[i]);
	}
}

void ABuildingPawn::CreateCurrentBlockSet(TSubclassOf<ABlockSet> pieceToBuild, FIntVector spawnIndexPosition, FRotator spawnRotation)
{
	if (!this->m_targetWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Targeted Building Found To Create Piece With"));
		return;
	}

	if (this->m_currentBlockSet != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant create another piece, another one is currently being made"));
		return;
	}

	if (!pieceToBuild)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid input piece class to create piece"));
		return;
	}

	// Create and spawn piece
	FActorSpawnParameters spawnInfo;
	spawnInfo.Owner = this;
	spawnInfo.Instigator = Instigator;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FIntVector indexSpawnPosition = spawnIndexPosition;
	FVector worldSpawnPosition = this->m_targetWorld->GetWorldPositionFromIndex(indexSpawnPosition);

	this->m_currentBlockSet = GetWorld()->SpawnActor<ABlockSet>(pieceToBuild, worldSpawnPosition, this->m_targetWorld->GetActorRotation(), spawnInfo);
	this->m_currentBlockSet->SetActorEnableCollision(false);
	float scale = this->m_targetWorld->blockScale;
	this->m_currentBlockSet->SetActorScale3D(FVector(scale, scale, scale));

	MoveCurrentBlockSet(indexSpawnPosition);
	RotateCurrentBlockSet(spawnRotation);
}

void ABuildingPawn::MoveCurrentBlockSet(FIntVector indexPosition)
{
	// Check piece
	if (this->m_currentBlockSet == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No set building block set to move."));
		return;
	}

	// Check Boundaries
	if (!this->m_targetWorld->IsInBoundaries(indexPosition))
	{
		return;
	}

	// Move Index Position and World Position
	this->m_currentBlockSet->MoveBuildingBlockSet(indexPosition, m_targetWorld->GetWorldPositionFromIndex(indexPosition));

	this->UpdatePlacementIndicator();
	this->SetActorLocation(this->m_targetWorld->GetWorldPositionFromIndex(this->m_currentBlockSet->GetRootWorldIndex()));
}

void ABuildingPawn::RotateCurrentBlockSet(FRotator rotation)
{
	// Check piece
	if (this->m_currentBlockSet == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No set block set to rotate."));
		return;
	}

	this->m_currentBlockSet->RotateBuildingBlockSet(rotation);

	this->UpdatePlacementIndicator();
}

void ABuildingPawn::ConfirmCurrentBlockSet()
{
	if (!this->ConfirmResourceRequirements(this->m_currentBlockSetClass))
	{
		GoToSelectMode();
		return;
	}
	
	if (!this->m_targetWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Targeted World Found To Confirm"));
		return;
	}

	if (!this->m_currentBlockSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid current BlockSet class to confirm BlockSet"));
		return;
	}

	if (this->m_targetWorld->AttachBlockSet(this->m_currentBlockSet))
	{
		// Set properties of blockset to attach it to the world
		this->m_currentBlockSet->SetMaterial(BUILDING_BLOCK_MAT_STANDARD);
		FIntVector savedIndexPosition = this->m_currentBlockSet->GetRootWorldIndex();
		FRotator savedRotation = this->m_currentBlockSet->GetBlockSetRotation();
		this->m_currentBlockSet->EnableBlockSet();
		this->m_currentBlockSet = nullptr;

		
		// Load New BlockSet to Create
		this->LoadBlockSet(this->m_currentBlockSetClass, savedIndexPosition, savedRotation);

		// Spend resources
		this->SpendResourceRequirements(this->m_currentBlockSetClass);
		// If not enough resource for another one. Boot back to menu
		if(!this->ConfirmResourceRequirements(this->m_currentBlockSetClass))
		{
			GoToSelectMode();
		}
	}
	else
	{
		// Dont do shit if couldnt confirm the current BlockSet
		UE_LOG(LogTemp, Display, TEXT("Couldn't confirm BlockSet"));
	}
}

void ABuildingPawn::CycleCurrentBlockSet(bool forward)
{
	
	if(forward)
		this->UIChangeBlockSetRight();
	else
		this->UIChangeBlockSetLeft();
}

void ABuildingPawn::UpdatePlacementIndicator()
{
	if (!this->m_targetWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid target world to update placement indicator with"));
		return;
	}

	if (!this->m_currentBlockSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid current BlockSet class to update placement indicator"));
		return;
	}

	if (this->m_targetWorld->CanAttachBlockSet(this->m_currentBlockSet))
		this->m_currentBlockSet->SetMaterial(BUILDING_BLOCK_MAT_PLACEABLE, this->m_materialIndicationBuildPlaceable);
	else
		this->m_currentBlockSet->SetMaterial(BUILDING_BLOCK_MAT_UNPLACEABLE, this->m_materialIndicationBuildNotPlaceable);

	return;
}

void ABuildingPawn::ShowCurrentBlockSet()
{
	this->m_currentBlockSet->SetActorHiddenInGame(false);
}

void ABuildingPawn::HideCurrentBlockSet()
{
	this->m_currentBlockSet->SetActorHiddenInGame(true);
}

#pragma endregion

#pragma region Delete Manipulation

void ABuildingPawn::MoveDeleteIndicator(FIntVector indexPosition)
{
	if (this->m_currentDeleteIndicator == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("No set delete indicator to move."));
		return;
	}
	
	// TODO Clean this shitty code
	this->SetBlockSetAtIndexToMaterial(m_currentDeleteIndicatorIndex, BUILDING_BLOCK_MAT_STANDARD);
	this->m_currentDeleteIndicator->SetActorLocation(this->m_targetWorld->GetWorldPositionFromIndex(indexPosition));
	this->m_currentDeleteIndicatorIndex = indexPosition;
	 ABlockSet* targetedBlockSet = this->m_targetWorld->GetBlockSetFromIndex(this->m_currentDeleteIndicatorIndex);
	
	if(IsValid(targetedBlockSet) && targetedBlockSet->PlayerCanDelete && this->m_buildingPawnState == DELETE_BLOCKSET)
		this->SetBlockSetAtIndexToMaterial(indexPosition, BUILDING_BLOCK_MAT_SHOW_DELETE);
}

bool ABuildingPawn::ConfirmDelete(FIntVector indexPosition)
{
	if (this->m_targetWorld == nullptr || !this->m_targetWorld->IsOccupied(indexPosition))
		return false;


	ABlockSet* toDelete = this->m_targetWorld->GetBlockSetFromIndex(indexPosition);
	if (!IsValid(toDelete) || !toDelete->PlayerCanDelete)
		return false;

	
	bool success = this->RefundResourceRequirements(toDelete->GetClass());
	if(!success)
	{
		return false;
	}
	success = this->m_targetWorld->DestroyBlockSet(indexPosition);
	if(!success)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy block."));
		return false;
	}
	
	return success;
}

bool ABuildingPawn::SetBlockSetAtIndexToMaterial(FIntVector indexPosition, EBuildingBlockSetMaterialState state)
{
	if (this->m_targetWorld == nullptr || !this->m_targetWorld->IsOccupied(indexPosition))
		return false;
	
	ABlockSet* blockSet = this->m_targetWorld->GetBlockSetFromIndex(indexPosition);
	if (blockSet == nullptr)
		return false;

	UMaterialInterface* material = nullptr;
	switch(state)
	{
		case BUILDING_BLOCK_MAT_UNPLACEABLE:
			material = this->m_materialIndicationBuildNotPlaceable;
			break;
		case BUILDING_BLOCK_MAT_PLACEABLE:
			material = this->m_materialIndicationBuildPlaceable;
			break;
		case BUILDING_BLOCK_MAT_SHOW_DELETE:
			material = this->m_materialIndicationDelete;
			break;
		case BUILDING_BLOCK_MAT_STANDARD:
			break;
		default: ;
	}
	
	blockSet->SetMaterial(state, material);
	return true;
}

void ABuildingPawn::EnableDeleteIndicator()
{
	if(m_currentDeleteIndicator == nullptr)
	{
		if(!IsValid(m_currentDeleteIndicatorClass))
		{
			UE_LOG(LogTemp, Error, TEXT("No set delete indicator class."));
			return;
		}

		FActorSpawnParameters spawnInfo;
		spawnInfo.Owner = this;
		spawnInfo.Instigator = Instigator;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		this->m_currentDeleteIndicator = GetWorld()->SpawnActor<AActor>(this->m_currentDeleteIndicatorClass, GetActorLocation(), FRotator::ZeroRotator, spawnInfo);
		this->m_currentDeleteIndicator->SetActorEnableCollision(false);

		FIntVector spawnIndex = FIntVector::ZeroValue;
		if(this->m_currentBlockSet != nullptr)
		{
			spawnIndex = this->m_currentBlockSet->GetRootWorldIndex();
		}
		else
		{
			if(this->m_targetWorld != nullptr)
				spawnIndex = this->m_targetWorld->GetWorldGridIndexSpawn(true);
		}
		this->MoveDeleteIndicator(spawnIndex);
	}

	this->m_currentDeleteIndicator->SetActorHiddenInGame(false);
}

void ABuildingPawn::DisableDeleteIndicator()
{
	if(this->m_currentDeleteIndicator != nullptr)
		this->m_currentDeleteIndicator->SetActorHiddenInGame(true);
}

#pragma endregion 

#pragma region Input Functions

void ABuildingPawn::ConfirmPressed()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET)
	{
		this->ConfirmCurrentBlockSet();
	}
	else if (this->m_buildingPawnState == SELECTING_BLOCKSET)
	{
		this->UIConfirmBlockSetSelection();
	}
	else if (this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		this->ConfirmDelete(this->m_currentDeleteIndicatorIndex);
	}

	return;
}

void ABuildingPawn::MoveXAxis(float axis)
{
	if (axis >= 0.9f)
	{
		this->m_elapsedTimeMoveXAxisInputDelay -= UGameplayStatics::GetWorldDeltaSeconds(this);
		if (this->m_elapsedTimeMoveXAxisInputDelay <= 0)
		{
			this->m_elapsedTimeMoveXAxisInputDelay = this->m_axisInputDelayRate;
			MoveXPos();
		}
	}
	else if (axis <= -0.9f)
	{
		this->m_elapsedTimeMoveXAxisInputDelay -= UGameplayStatics::GetWorldDeltaSeconds(this);
		if (this->m_elapsedTimeMoveXAxisInputDelay <= 0)
		{
			this->m_elapsedTimeMoveXAxisInputDelay = this->m_axisInputDelayRate;
			MoveXNeg();
		}
	}
	else
	{
		this->m_elapsedTimeMoveXAxisInputDelay = 0.0f;
	}
}

void ABuildingPawn::MoveYAxis(float axis)
{
	if (axis >= 1.0f)
	{
		this->m_elapsedTimeMoveYAxisInputDelay -= UGameplayStatics::GetWorldDeltaSeconds(this);
		if (this->m_elapsedTimeMoveYAxisInputDelay <= 0)
		{
			this->m_elapsedTimeMoveYAxisInputDelay = this->m_axisInputDelayRate;
			MoveYNeg();
		}
	}
	else if (axis <= -1.0f)
	{
		this->m_elapsedTimeMoveYAxisInputDelay -= UGameplayStatics::GetWorldDeltaSeconds(this);
		if (this->m_elapsedTimeMoveYAxisInputDelay <= 0)
		{
			this->m_elapsedTimeMoveYAxisInputDelay = this->m_axisInputDelayRate;
			MoveYPos();
		}
	}
	else
	{
		this->m_elapsedTimeMoveYAxisInputDelay = 0.0f;
	}
}

void ABuildingPawn::MoveXNeg()
{
	if(this->m_buildingPawnState == PLACING_BLOCKSET || this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
		{
			FIntVector right = this->GetCameraRightVector();
			FIntVector moveOffset = FIntVector(right.X*-1, right.Y*-1, 0);
			this->MoveCurrentBlockSet(m_currentBlockSet->GetRootWorldIndex() + moveOffset);
			this->MoveDeleteIndicator(this->m_currentDeleteIndicatorIndex + moveOffset);
		}
	}
	else if (this->m_buildingPawnState == SELECTING_BLOCKSET)
	{
		this->UIChangeBlockSetLeft();
	}
}

void ABuildingPawn::MoveXPos()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET || this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
		{
			FIntVector right = this->GetCameraRightVector();
			FIntVector moveOffset = FIntVector(right.X, right.Y, 0);
			this->MoveCurrentBlockSet(m_currentBlockSet->GetRootWorldIndex() + moveOffset);
			this->MoveDeleteIndicator(this->m_currentDeleteIndicatorIndex + moveOffset);
		}
	}
	else if (this->m_buildingPawnState == SELECTING_BLOCKSET)
	{
		this->UIChangeBlockSetRight();
	}
}

void ABuildingPawn::MoveYNeg()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET || this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
		{
			FIntVector forward = this->GetCameraForwardVector();
			FIntVector moveOffset = FIntVector(forward.X, forward.Y, 0);
			this->MoveCurrentBlockSet(m_currentBlockSet->GetRootWorldIndex() + moveOffset);
			this->MoveDeleteIndicator(this->m_currentDeleteIndicatorIndex + moveOffset);
		}
	}
	else if (this->m_buildingPawnState == SELECTING_BLOCKSET)
	{
		this->UIChangeBlockSetDown();
	}
}

void ABuildingPawn::MoveYPos()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET || this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
		{
			FIntVector forward = this->GetCameraForwardVector();
			FIntVector moveOffset = FIntVector(forward.X * -1, forward.Y * -1, 0);
			this->MoveCurrentBlockSet(m_currentBlockSet->GetRootWorldIndex() + moveOffset);
			this->MoveDeleteIndicator(this->m_currentDeleteIndicatorIndex + moveOffset);
		}
	}
	else if (this->m_buildingPawnState == SELECTING_BLOCKSET)
	{
		this->UIChangeBlockSetUp();
	}
}

void ABuildingPawn::MoveBlockSetZNeg()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET || this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
		{
			FIntVector moveOffset = FIntVector(0, 0, -1);
			this->MoveCurrentBlockSet(this->m_currentBlockSet->GetRootWorldIndex() + moveOffset);
			this->MoveDeleteIndicator(this->m_currentDeleteIndicatorIndex + moveOffset);
		}
	}
}

void ABuildingPawn::MoveBlockSetZPos()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET || this->m_buildingPawnState == DELETE_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
		{
			FIntVector moveOffset = FIntVector(0, 0, 1);
			this->MoveCurrentBlockSet(this->m_currentBlockSet->GetRootWorldIndex() + moveOffset);
			this->MoveDeleteIndicator(this->m_currentDeleteIndicatorIndex + moveOffset);
		}
	}
}

void ABuildingPawn::RotateBlockSetZAxis(float axis)
{
	if (axis >= 1.0f)
	{
		this->m_elapsedTimeRotateZAxisInputDelay -= UGameplayStatics::GetWorldDeltaSeconds(this);
		if(this->m_elapsedTimeRotateZAxisInputDelay <= 0)
		{
			this->m_elapsedTimeRotateZAxisInputDelay = this->m_axisInputDelayRate;
			RotateBlockSetZClockwise();
		}
	}
	else if (axis <= -1.0f)
	{
		this->m_elapsedTimeRotateZAxisInputDelay -= UGameplayStatics::GetWorldDeltaSeconds(this);
		if (this->m_elapsedTimeRotateZAxisInputDelay <= 0)
		{
			this->m_elapsedTimeRotateZAxisInputDelay = this->m_axisInputDelayRate;
			RotateBlockSetsZCounterClockwise();
		}
	}
	else
	{
		this->m_elapsedTimeRotateZAxisInputDelay = 0.0f;
	}
}

void ABuildingPawn::RotateBlockSetZClockwise()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
			this->RotateCurrentBlockSet(FRotator(0.0f, 90.0f, 0.0f));
	}
}

void ABuildingPawn::RotateBlockSetsZCounterClockwise()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET)
	{
		if (this->m_currentBlockSet != nullptr)
			this->RotateCurrentBlockSet(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void ABuildingPawn::CycleBlockSetLeft()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET)
	{
		this->CycleCurrentBlockSet(false);
	}
	
}

void ABuildingPawn::CycleBlockSetRight()
{
	if (this->m_buildingPawnState == PLACING_BLOCKSET)
	{
		this->CycleCurrentBlockSet(true);
	}
}

#pragma region Input Helper Functions

//TODO These inputs do not handle a rotate building well
FIntVector ABuildingPawn::GetCameraForwardVector()
{
	TArray<UCameraComponent*> cameraComponents;
	this->GetComponents(cameraComponents);
	if (cameraComponents.Num() > 0)
	{
		UCameraComponent* camera = cameraComponents[0];

		// Transform camera forward vector for calculation
		FVector cameraForwardTranslatedToBuilding = this->m_targetWorld->GetActorLocation() + camera->GetForwardVector();
		FVector cameraForwardinBuildingSpace = this->m_targetWorld->GetTransform().InverseTransformPosition(cameraForwardTranslatedToBuilding);
		FVector cameraForwardNormalized = cameraForwardinBuildingSpace;
		cameraForwardNormalized.Normalize();

		FIntVector directionRounded = FIntVector(FMath::RoundToInt(cameraForwardNormalized.X), FMath::RoundToInt(cameraForwardNormalized.Y), FMath::RoundToInt(cameraForwardNormalized.Z));

		if (directionRounded.X == 1 && directionRounded.Y == 1)
			directionRounded.Y = 0;


		return directionRounded;
	}

	return FIntVector::ZeroValue;
}

//TODO These inputs do not handle a rotate building well
FIntVector ABuildingPawn::GetCameraRightVector()
{
	TArray<UCameraComponent*> cameraComponents;
	this->GetComponents(cameraComponents);
	if (cameraComponents.Num() > 0)
	{
		UCameraComponent* camera = cameraComponents[0];

		// Transform camera forward vector for calculation
		FVector cameraRightTranslatedToBuilding = this->m_targetWorld->GetActorLocation() + camera->GetRightVector();
		FVector cameraRightinBuildingSpace = this->m_targetWorld->GetTransform().InverseTransformPosition(cameraRightTranslatedToBuilding);
		FVector cameraRightNormalized = cameraRightinBuildingSpace;
		cameraRightNormalized.Normalize();

		FIntVector directionRounded = FIntVector(FMath::RoundToInt(cameraRightNormalized.X), FMath::RoundToInt(cameraRightNormalized.Y), FMath::RoundToInt(cameraRightNormalized.Z));

		if (directionRounded.X != 0 && directionRounded.Y != 0)
			directionRounded.Y = 0;

		return directionRounded;
	}

	return FIntVector::ZeroValue;
}


#pragma endregion

#pragma endregion





