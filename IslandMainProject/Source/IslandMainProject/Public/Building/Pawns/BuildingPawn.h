// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IslandMainProjectGameModeBase.h"
#include "BuildingLibrary.h"

#include "BuildingPawn.generated.h"



class ABlockSet;

UENUM(BlueprintType, Category="Building Pawn")
enum EBuildingPawnState
{
	NOT_SHOWN UMETA(DisplayName = "NONE"),
	SELECTING_BLOCKSET UMETA( DisplayName = "SELECTING BLOCK SETS"),
	OBSERVING_BLOCKSET UMETA(DisplayName = "OBSERVING BLOCK SETS"),
	PLACING_BLOCKSET UMETA(DisplayName = "PLACING BLOCK SETS"),
	DELETE_BLOCKSET UMETA(DisplayName = "DELETE BLOCK SETS")
};

enum EBuildingBlockSetMaterialState;

UCLASS()
class ISLANDMAINPROJECT_API ABuildingPawn : public APawn
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Root)
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* PrimaryCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USpringArmComponent* PrimaryCameraBoom;


	
	// Sets default values for this pawn's properties
	ABuildingPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// UI interaction blueprint functions
	UFUNCTION(BlueprintImplementableEvent)
	void UIShowSelectionMenu();
	UFUNCTION(BlueprintImplementableEvent)
	void UIHideSelectionMenu();
	UFUNCTION(BlueprintImplementableEvent)
	void UIConfirmBlockSetSelection();
	UFUNCTION(BlueprintImplementableEvent)
	void UIChangeBlockSetUp();
	UFUNCTION(BlueprintImplementableEvent)
	void UIChangeBlockSetDown();
	UFUNCTION(BlueprintImplementableEvent)
	void UIChangeBlockSetLeft();
	UFUNCTION(BlueprintImplementableEvent)
	void UIChangeBlockSetRight();
	UFUNCTION(BlueprintImplementableEvent)
	void UILoadBlockSetOptions(const TArray<FBlockSetData>& options, class UInventoryComponent* inventory);
	UFUNCTION(BlueprintCallable)
	void UIOnLoadSelectedBlockSet(TSubclassOf<ABlockSet> selectedBlockSetClass);

	// Util Functions
	UFUNCTION(BlueprintCallable)
	bool ConfirmResourceRequirements(TSubclassOf<ABlockSet> selectedBlockSetClass);
	UFUNCTION(BlueprintCallable)
	bool SpendResourceRequirements(TSubclassOf<ABlockSet> selectedBlockSetClass);
	UFUNCTION(BlueprintCallable)
	bool RefundResourceRequirements(TSubclassOf<ABlockSet> selectedBlockSetClass);
	
	// Mode Change and Input
	UFUNCTION(BlueprintCallable)
	void GoToMode(EBuildingPawnState newPawnState);
	UFUNCTION(BlueprintCallable)
	void ToggleMode();
	UFUNCTION(BlueprintCallable)
	void GoToSelectMode();
	UFUNCTION(BlueprintCallable)
	void GoToObserveMode();
	UFUNCTION(BlueprintCallable)
	void GoToPlacementMode();
	UFUNCTION(BlueprintCallable)
	void GoToDeleteMode();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Input
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Initalize Building")
	void SetTargetWorldAndInitialize(AWorldBuildingGrid* targetWorldGrid);

	UFUNCTION(BlueprintCallable, Category = "Enter Building")
	void EnterBuildingMode();

	UFUNCTION(BlueprintCallable, Category = "Exit Building")
	void ExitBuildingMode();
	
private:
	
	// Stored pointers to a game mode class
	UPROPERTY()
	AIslandMainProjectGameModeBase* m_gameMode;

	// Building State
	UPROPERTY(VisibleAnywhere, Category = "Building", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EBuildingPawnState> m_buildingPawnState;
	UPROPERTY(VisibleAnywhere, Category = "Building", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EBuildingPawnState> m_lastBuildingState;;

	// Build Target
	UPROPERTY(EditInstanceOnly, Category = "Building", meta = (AllowPrivateAccess = "true"))
	AWorldBuildingGrid* m_targetWorld;

	// Block Set Map
	TMap<TSubclassOf<ABlockSet>, FBlockSetData> m_blockSetOptionsMap;
	
	// Current Block Set
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABlockSet> m_currentBlockSetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Building", meta = (AllowPrivateAccess = "true"))
	ABlockSet* m_currentBlockSet;

	// Current Delete Indicator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Delete", meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "1.0"))
	float m_refundRatio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Delete", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> m_currentDeleteIndicatorClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building Delete", meta = (AllowPrivateAccess = "true"))
	AActor* m_currentDeleteIndicator;
	FIntVector m_currentDeleteIndicatorIndex;

	// Material For Manipulation
	UPROPERTY(EditDefaultsOnly, Category = "Block Set Material Properties")
	class UMaterialInterface* m_materialIndicationBuildPlaceable;
	UPROPERTY(EditDefaultsOnly, Category = "Block Set Material Properties")
	class UMaterialInterface* m_materialIndicationBuildNotPlaceable;
	UPROPERTY(EditDefaultsOnly, Category = "Block Set Material Properties")
	class UMaterialInterface* m_materialIndicationDelete;

	
	// Block Set manipulation
	void LoadBlockSet(TSubclassOf<ABlockSet> blockSet, FIntVector spawnBuildingIndex, FRotator spawnRotation);
	void LoadBlockSetMap();
	void CreateCurrentBlockSet(TSubclassOf<ABlockSet> blockSetToBuild, FIntVector spawnIndexPosition, FRotator spawnRotation);
	void MoveCurrentBlockSet(FIntVector position);
	void RotateCurrentBlockSet(FRotator position);
	void ConfirmCurrentBlockSet();
	void CycleCurrentBlockSet(bool forward);

	// Indicators for current block set being built
	void UpdatePlacementIndicator();
	void ShowCurrentBlockSet();
	void HideCurrentBlockSet();

	// Delete manipulation
	bool ConfirmDelete(FIntVector indexPosition);

	// Indicators for Delete
	void MoveDeleteIndicator(FIntVector indexPosition);
	bool SetBlockSetAtIndexToMaterial(FIntVector indexPosition, EBuildingBlockSetMaterialState state);
	

	// Delete Indicator
	void EnableDeleteIndicator();
	void DisableDeleteIndicator();

	// Input Properties
	UPROPERTY(EditDefaultsOnly, Category = "Building Pawn Input Properties")
	float m_axisInputDelayRate = 1.0f;
	float m_elapsedTimeMoveXAxisInputDelay = 0.0f;
	float m_elapsedTimeMoveYAxisInputDelay = 0.0f;
	float m_elapsedTimeRotateZAxisInputDelay = 0.0f;
	
	// Block Set Input functions;
	void ConfirmPressed();
	void MoveXAxis(float axis);
	void MoveYAxis(float axis);
	void MoveXNeg();
	void MoveXPos();
	void MoveYNeg();
	void MoveYPos();
	void MoveBlockSetZNeg();
	void MoveBlockSetZPos();
	void RotateBlockSetZAxis(float axis);
	void RotateBlockSetZClockwise();
	void RotateBlockSetsZCounterClockwise();
	void CycleBlockSetLeft();
	void CycleBlockSetRight();

	// Helper Input
	FIntVector GetCameraForwardVector();
	FIntVector GetCameraRightVector();

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	class UCameraControlComponent* CameraControlComp;
};
