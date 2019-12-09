// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumptionComponent.h"
#include "Public/InventoryComponent.h"
#include "Public/Building/Actors/BlockSet.h"
#include "ReceiverComponent.h"

// Sets default values for this component's properties
UConsumptionComponent::UConsumptionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UConsumptionComponent::SetConsumptionProperties(UInventoryComponent* consumptionInventory)
{
	if (IsValid(consumptionInventory))
		this->m_consumptionInventory = consumptionInventory;
}


// Called when the game starts
void UConsumptionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// Get Properties Row
	if (ConsumptionPropertiesDataTableRow.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Sender Component Not Configured With Transfer Properties: %s"), *this->GetOwner()->GetName());
		return;
	}
	else
	{
		this->m_consumptionPropertiesData = *ConsumptionPropertiesDataTableRow.GetRow<FConsumptionPropertiesRow>("");
	}


	// Set Tick Requirements
	TArray<UActorComponent*> components = this->GetOwner()->GetComponentsByClass(UReceiverComponent::StaticClass());
	for (UActorComponent* preReqComponent : components)
	{
		this->AddTickPrerequisiteComponent(preReqComponent);
	}
}


// Called every frame
void UConsumptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Confirm inventory is valid
	if (!IsValid(this->m_consumptionInventory))
		return;
	// ...
	// Confirm component is active
	switch(this->m_activeStatus)
	{
	case EComponentActiveStatus::ENABLED:
		// Confirm whether it has enough resource to continue
		{
			this->m_elapsedTime -= DeltaTime;

			if (this->m_elapsedTime <= 0.0f)
			{
				this->m_elapsedTime = this->m_consumptionPropertiesData.RateInSeconds;

				bool hasEnough = this->m_consumptionInventory->HasEnoughItems(this->m_consumptionPropertiesData.Items);
				if(hasEnough)
				{
					this->m_consumptionInventory->RemoveFromInventory(this->m_consumptionPropertiesData.Items);
				}
				else
				{
					ABlockSet* blockSetOwner = Cast<ABlockSet>(this->GetOwner());
					blockSetOwner->DisableBlockSet();
				}
			}
			break;
		}
	case EComponentActiveStatus::DISABLED:
		// Confirm whether it has enough resource to re enable
		{
			bool hasEnough = this->m_consumptionInventory->HasEnoughItems(this->m_consumptionPropertiesData.Items);
			if (hasEnough)
			{
				ABlockSet* blockSetOwner = Cast<ABlockSet>(this->GetOwner());
				blockSetOwner->EnableBlockSet();
			}
			else
			{
				
			}
			break;
		}
		default: ;
	}
}

