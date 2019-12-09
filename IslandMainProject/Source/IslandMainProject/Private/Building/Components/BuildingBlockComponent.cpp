// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Building/Components/BuildingBlockComponent.h"
#include "Public/Building/Actors/BlockSet.h"


// Sets default values for this component's properties
UBuildingBlockComponent::UBuildingBlockComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UBuildingBlockComponent::GenerateIndexes()
{
	FIntVector offset = this->Offset;

	this->IndexPositions.Empty();

	for (size_t x = 0; x < this->DimensionX; x++)
	{
		for (size_t y = 0; y < this->DimensionY; y++)
		{
			for (size_t z = 0; z < this->DimensionZ; z++)
			{
				FBuildingIndex buildIndex = FBuildingIndex();
				buildIndex.LocalIndex = FIntVector(x, y, z) + offset;
				buildIndex.WorldIndex = FIntVector(x, y, z) + offset;
				this->IndexPositions.Add(buildIndex.LocalIndex, buildIndex);
			}
		}
	}
	this->LocalRootIndex = this->Offset;
	
	// Attempts to force reload of object
	/*AActor* test = this->GetOwner();
	test->ExecuteConstruction(test->GetTransform(), nullptr, nullptr, true);*/
	
}

TMap<FIntVector, FBuildingIndex> UBuildingBlockComponent::GetIndexPositions() const
{
	return this->IndexPositions;
}

ABlockSet* UBuildingBlockComponent::GetBlockSetParent() const
{
	return Cast<ABlockSet>(this->GetOwner());
}

bool UBuildingBlockComponent::GetLocalRootIndex(FBuildingIndex & o_rootIndex) const
{
	if (this->IndexPositions.Contains(this->LocalRootIndex))
	{
		o_rootIndex = this->IndexPositions[this->LocalRootIndex];
		return true;
	}
	else
	{
		return false;
	}
}

void UBuildingBlockComponent::MoveIndexesRelativeToWorldIndex(FIntVector worldIndex)
{
	for (auto& element : this->IndexPositions)
	{
		element.Value.WorldIndex = worldIndex + element.Value.LocalIndex;
	}
}

void UBuildingBlockComponent::RotateIndexesRelativeToRotation(FBuildingIndex indexOfRotation, FRotator rotation)
{
	for (auto& element : this->IndexPositions)
	{
		FIntVector relativeLocalIndexPosition = element.Value.LocalIndex - indexOfRotation.LocalIndex;
		FVector fRelativeLocalIndexPosition = FVector(relativeLocalIndexPosition.X, relativeLocalIndexPosition.Y, relativeLocalIndexPosition.Z);
		fRelativeLocalIndexPosition = rotation.RotateVector(fRelativeLocalIndexPosition);

		element.Value.LocalIndex = FIntVector(FMath::RoundToInt(fRelativeLocalIndexPosition.X), FMath::RoundToInt(fRelativeLocalIndexPosition.Y), FMath::RoundToInt(fRelativeLocalIndexPosition.Z));
		element.Value.WorldIndex = indexOfRotation.WorldIndex + element.Value.LocalIndex;
	}
}

void UBuildingBlockComponent::PostLoad()
{
	Super::PostLoad();
}

// Called when the game starts
void UBuildingBlockComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

bool UBuildingBlockComponent::GetAllBlocksWorldIndices(TArray<FIntVector>& o_worldIndices) const
{
	int amount = 0;
	for (auto It = IndexPositions.CreateConstIterator(); It; ++It)
	{
		o_worldIndices.Add(It->Value.WorldIndex);
		amount++;
	}
	return amount != 0;
}

