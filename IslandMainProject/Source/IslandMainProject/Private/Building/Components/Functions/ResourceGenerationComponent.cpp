// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceGenerationComponent.h"
#include "BlockSet.h"
#include "Public/InventoryComponent.h"

// Sets default values for this component's properties
UResourceGenerationComponent::UResourceGenerationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	// ...
}


// Called when the game starts
void UResourceGenerationComponent::BeginPlay()
{
	Super::BeginPlay();
	if (m_buildingSetParent)
	{
		m_inventory = (UInventoryComponent*)m_buildingSetParent->GetComponentByClass(UInventoryComponent::StaticClass());
	}
	// ...
	if (GenerationPropertiesDataTableRow.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Generation Component Not Configured With Generation Properties: %s"), *this->GetOwner()->GetName());
		return;
	}
	else
	{
		m_generationPropertiesRow = *GenerationPropertiesDataTableRow.GetRow<FGenerationPropertiesRow>("");
	}
	
}

void UResourceGenerationComponent::EnableBlockSetComponent()
{
	Super::EnableBlockSetComponent();
}

void UResourceGenerationComponent::DisableBlockSetComponent()
{
	Super::DisableBlockSetComponent();
}


// Called every frame
void UResourceGenerationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (m_buildingSetParent && this->m_activeStatus == EComponentActiveStatus::ENABLED && m_inventory)
	{
		this->m_elapsedTime -= DeltaTime;

		if(this->m_elapsedTime <= 0.0f)
		{
			this->m_elapsedTime = this->m_generationPropertiesRow.RateInSeconds;

			// For each resource add
			for (FItemAmount itemAmount : this->m_generationPropertiesRow.Items.ItemAmounts)
			{
				int currentamount = this->m_inventory->GetItemAmount(itemAmount.ItemRow.RowName);
				if (currentamount < itemAmount.MaxStack)
				{
					this->m_inventory->AddItem(itemAmount.ItemRow.RowName, itemAmount.Amount);
					factorystate = EFactoryState::E_ISNOTFULL;
				}
				else
				{
					factorystate = EFactoryState::E_ISFULL;
				}
			}
		}
	}
	// ...
}

