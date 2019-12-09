#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/Building/Components/BuildingBlockComponent.h"
#include "BuildingLibrary.h"
#include "WorldBuildingGrid.generated.h"

class ABlockSet;
UCLASS()
class ISLANDMAINPROJECT_API AWorldBuildingGrid : public AActor
{
	GENERATED_BODY()

public:

	static const float blockSize;
	
	// Editable Defaults

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Attachment Requirement Properties", meta = (AllowPrivateAccess = "true"))
		TArray<TEnumAsByte<EObjectTypeQuery>> BlockSetAttachmentObjectTypeCollisionFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Attachment Requirement Properties", meta = (AllowPrivateAccess = "true"))
		TArray<UClass*> BlockSetAttachmentClassCollisionFilter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Properties")
		FIntVector MiddleBlockDimensions = FIntVector(3, 3, 6);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Properties")
		int EdgeBlockSize = 1;

	
		

	// TODO Thinking about this setting. I'm thinking I might deprecate it. Currently its broken but it also doesn't help in terms of level building. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Grid Properties")
		float blockScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Properties")
		FVector blockCenterOffset = FVector(0.0f, 0.0f, 0.0f);

	// Sets default values for this actor's properties
	AWorldBuildingGrid();

	// Accessors

	UFUNCTION(BlueprintCallable, Category = "World Inventory")
		class UInventoryComponent* GetBuildingInventoryCompoonent() const;

	// Primary function for attaching block sets
	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool AttachBlockSet(ABlockSet* blockSet, bool ignoreAttachmentRequirements = false);

	// Primary function for deleteing block sets
		bool DestroyBlockSet(FIntVector index);

	// Block Set Checks
	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool CanAttachBlockSet(ABlockSet* blockSet, bool ignoreAttachmentRequirements = false) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool IsOccupied(FIntVector index) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool IsInBoundaries(FIntVector index) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool IsOutsideMiddle(FIntVector index) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool IsOnEdgeZone(FIntVector index) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Function")
		bool IsAttachableAdjacent(FIntVector index) const;

	inline bool IsIndexAttachable(FIntVector index) const
	{
		return (m_worldMap.Contains(index) && m_worldMap[index]->BlockType == EBlockType::ATTACHABLE);
	}

	// Positional Based Functions
	UFUNCTION(BlueprintCallable, Category = "World Grid Position")
		TSet<ABlockSet*>  GetBuildingBlockSetInDistance(FIntVector worldIndexPosition, FIntVector Distance);

	UFUNCTION(BlueprintCallable, Category = "World Grid Position")
		FVector GetWorldPositionFromIndex(FIntVector worldIndexPosition) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Position")
		ABlockSet* GetBlockSetFromIndex(FIntVector worldIndexPosition) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Position")
		FVector GetWorldPositionFromFloor(int floor) const;

	UFUNCTION(BlueprintCallable, Category = "World Grid Position")
		FIntVector GetWorldIndexPositionFromApproximatedWorldPosition(FVector worldPosition);

	UFUNCTION(BlueprintCallable, Category = "World Grid Position")
		FIntVector GetWorldGridIndexSpawn(bool positiveZ) const;


	// Load Functions
	UFUNCTION(BlueprintCallable, Category = "World Grid Loading")
		bool LoadPreExistingBlockSets();
	
	// Clean up functions
	UFUNCTION(BlueprintCallable, Category = "World Grid Clean up")
		void ClearWorldMapsByWorldIndices(const TArray<FIntVector>& i_worldIndices);
	UFUNCTION(BlueprintCallable, Category = "World Grid Clean up")
		bool HasNullBlockComponentsInMap() const;
protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// This method is for initializing the building called from Begin Play. Will eventually deal with loading the 
	// building from a save file of some sort
	void Initialize();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Grid Private Properties", meta = (AllowPrivateAccess = "true"))
		USceneComponent* BlockSetParent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Loading", meta = (AllowPrivateAccess = "true"))
	 bool FindAllBlockSetsInMap = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid Loading", meta = (AllowPrivateAccess = "true"))
	TArray<ABlockSet*> PreExistingBlockSets;

	// The spacial blocks of the map. Null pts mean a block isn't there
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Grid Private Properties", meta = (AllowPrivateAccess = "true"))
		TMap<FIntVector, class UBuildingBlockComponent*> m_worldMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Inventory", meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* WorldInventory;
};

