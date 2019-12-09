// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Building/Components/Core/BuildingTechTreeComponent.h"

// Sets default values for this component's properties
UBuildingTechTreeComponent::UBuildingTechTreeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBuildingTechTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



TArray<FBlockSetData> UBuildingTechTreeComponent::GetPossibleBlockSets() const
{
	/*TMap<FName, FBlockSetData> m_blockSetMap;
	for (size_t i = 0; i < m_possibleBuildingBlockSets.Num(); i++)
	{
		m_blockSetMap.Add(m_possibleBuildingBlockSets[i].GetKeyName(), m_possibleBuildingBlockSets[i]);
	}
*/
	return this->m_possibleBuildingBlockSets;
}
