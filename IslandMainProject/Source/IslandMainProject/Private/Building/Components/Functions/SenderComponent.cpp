// Fill out your copyright notice in the Description page of Project Settings.

#include "Functions/SenderComponent.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/World.h"
#include "Public/Building/Actors/WorldBuildingGrid.h"
#include "Public/Building/Actors/BlockSet.h"
#include "Functions/ReceiverComponent.h"
#include "Public/Building/Components/Functions/ResourceGenerationComponent.h"

// Sets default values for this component's properties
USenderComponent::USenderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool USenderComponent::RegisterReceiverTarget(UReceiverComponent* target)
{
	if (!IsValid(target))
		return false;
	
	if (m_registeredReceivers.Contains(target))
		return false;

	if (this->m_activeStatus != EComponentActiveStatus::ENABLED)
		return false;

	if (target->GetTransferPropertiesRow().RowName != this->TransferPropertiesDataTableRow.RowName)
		return false;
	
	m_registeredReceivers.Add(target);
	return true;
}

bool USenderComponent::UnRegisterReceiverTarget(UReceiverComponent* target)
{
	if (!IsValid(target))
		return false;
	
	if (!m_registeredReceivers.Contains(target))
		return false;

	m_registeredReceivers.Remove(target);
	return true;
}

void USenderComponent::NotifyReceiveActors(ESenderReceiverMessageType type)
{
	int range = m_transferPropertiesData.BlockIndexRange;
	
	AActor* owner = this->GetOwner();
	AIslandMainProjectGameModeBase* gameMode = Cast<AIslandMainProjectGameModeBase>(owner->GetWorld()->GetAuthGameMode());
	AWorldBuildingGrid * worldGrid = gameMode->GetWorldGridActor();

	ABlockSet* blockSetOwner = Cast<ABlockSet>(owner);
	TSet<ABlockSet*> blockSetsInRange = worldGrid->GetBuildingBlockSetInDistance(blockSetOwner->GetRootWorldIndex(), FIntVector(range, range, range));
	blockSetsInRange.Remove(blockSetOwner);


	for(ABlockSet* blockSet : blockSetsInRange)
	{
		TArray<UReceiverComponent*> receivers;
		blockSet->GetComponents<UReceiverComponent>(receivers, true);

		if(receivers.Num() > 0)
		{
			for(UReceiverComponent* receiver : receivers)
			{
				// Only accept receivers that have a matching row
				if (receiver->GetTransferPropertiesRow().RowName != this->TransferPropertiesDataTableRow.RowName)
					continue;
				
				switch (type) {
				case ESenderReceiverMessageType::REGISTER_SENDER:
					{
						bool success = receiver->RegisterSender(this);
						if (success)
						{
							m_registeredReceivers.Add(receiver);
						}
						break;
					}
				case ESenderReceiverMessageType::UNREGISTER_SENDER:
					{
						bool success = receiver->UnRegisterSender(this);
						if (success)
						{
							m_registeredReceivers.Remove(receiver);
						}
						break;
					}
				default:;
				}
			}
		}
	}

	switch(type)
	{
	case ESenderReceiverMessageType::REGISTER_SENDER:
		break;
	case ESenderReceiverMessageType::UNREGISTER_SENDER:
		if(m_registeredReceivers.Num() > 0)
			UE_LOG(LogTemp, Error, TEXT("Failed to Unregister all Receivers: %s"), *this->GetOwner()->GetName());
		break;
	default: ;
	}
	
	
}

FDataTableRowHandle USenderComponent::GetTransferPropertiesRow() const
{
	return this->TransferPropertiesDataTableRow;
}

void USenderComponent::SetSenderProperties(UInventoryComponent* senderInventory)
{
	if(IsValid(senderInventory))
		this->m_senderInventory = senderInventory;
}


void USenderComponent::EnableBlockSetComponent()
{
	Super::EnableBlockSetComponent();

	// Enable Function
	this->NotifyReceiveActors(ESenderReceiverMessageType::REGISTER_SENDER);

}

void USenderComponent::DisableBlockSetComponent()
{	
	Super::DisableBlockSetComponent();

	// Disable Function
	this->NotifyReceiveActors(ESenderReceiverMessageType::UNREGISTER_SENDER);
}


// Called when the game starts
void USenderComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get Properties Row
	if(TransferPropertiesDataTableRow.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Sender Component Not Configured With Transfer Properties: %s"), *this->GetOwner()->GetName());
		return;
	}
	else
	{
		m_transferPropertiesData = *TransferPropertiesDataTableRow.GetRow<FTransferPropertiesRow>("");
	}

	// Set Tick Requirements
	TArray<UActorComponent*> components = this->GetOwner()->GetComponentsByClass(UResourceGenerationComponent::StaticClass());
	for (UActorComponent* preReqComponent : components)
	{
		this->AddTickPrerequisiteComponent(preReqComponent);
	}

	// Notify Receivers In Range
	this->NotifyReceiveActors(ESenderReceiverMessageType::REGISTER_SENDER);
}

void USenderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Confirm component is active
	if (this->m_activeStatus != EComponentActiveStatus::ENABLED)
		return;

	// Confirm inventory is valid
	if(!IsValid(this->m_senderInventory))
		return;

	
	this->m_elapsedTime -= DeltaTime;

	if (this->m_elapsedTime <= 0.0f)
	{
		this->m_elapsedTime = this->m_transferPropertiesData.RateInSeconds;

		// For each receiver, send items
		for(UReceiverComponent* receiver : this->m_registeredReceivers)
		{
			// Confirm receiver is valid
			if (!IsValid(receiver))
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid Receiver Found In Sender: %s"), *this->GetOwner()->GetName());
				continue;
			}

			// Check sender has enough of each resource to send
			if(!m_senderInventory->HasEnoughItems(this->m_transferPropertiesData.Items))
			{
				return;
			}

			// Remove from this inventory
			m_senderInventory->RemoveFromInventory(this->m_transferPropertiesData.Items);
			// Add to receiver inventory
			receiver->ReceiveItems(this);
			
		}
	}
}

