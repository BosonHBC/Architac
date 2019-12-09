// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingBlockComponent.generated.h"


USTRUCT(BlueprintType)
struct FBuildingIndex
{
	GENERATED_USTRUCT_BODY()
public:

	FBuildingIndex()
	{
		
	}
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntVector LocalIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntVector WorldIndex;
};

UENUM(BlueprintType, Category="Block Component Type")
enum class EBlockType : uint8
{
	ATTACHABLE UMETA(DisplayName = "Attachable"),  // Means things can be attach next to it, and it takes up all 6 sides of space
	NOT_ATTACHABLE UMETA(DisplayName = "Not Attachable"), // Means things can't attach to it, and it generically takes up the space of the tile
	OVERLAP UMETA(DisplayName = "Overlap"), // Means it is just used to detect things that could take up space, this does not take up space
};

// TODO NEED TO BE RENAMED. Now that we have environemntals tihs should be called -> UBlockComponent
UCLASS( ClassGroup=(BlockSetComponent), Blueprintable, meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UBuildingBlockComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
	EBlockType BlockType = EBlockType::ATTACHABLE;

	// Sets default values for this component's properties
	UBuildingBlockComponent();

	// Getters
	TMap<FIntVector, FBuildingIndex> GetIndexPositions() const;

	UFUNCTION(BlueprintCallable)
	class ABlockSet* GetBlockSetParent() const;

	// Returns success on properly configured root index
	UFUNCTION(BlueprintCallable)
	bool GetLocalRootIndex(FBuildingIndex & o_rootIndex) const;

	void MoveIndexesRelativeToWorldIndex(FIntVector worldIndex);

	void RotateIndexesRelativeToRotation(FBuildingIndex indexOfRotation,FRotator rotation);

protected:
	// Properties for generating indexes
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Properties Index Generation", meta = (UIMin = 1))
		int DimensionX = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Properties Index Generation", meta = (UIMin = 1))
		int DimensionY = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Properties Index Generation", meta = (UIMin = 1))
		int DimensionZ = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Properties Index Generation")
		FIntVector Offset;
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Block Properties Index Generation")
		void GenerateIndexes();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Properties")
		FIntVector LocalRootIndex;
	
	virtual void PostLoad() override;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintCallable, Category = "Block Properties")
		bool GetAllBlocksWorldIndices(TArray<FIntVector>& o_worldIndices) const;
	
private:

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
		FIntVector LocalIndexPosition;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
		TMap<FIntVector,FBuildingIndex> IndexPositions; // Index Positions Mapped by local index

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Properties", meta = (AllowPrivateAccess = "true"))
		TArray<FIntVector> WorldIndexPositions;*/
};
