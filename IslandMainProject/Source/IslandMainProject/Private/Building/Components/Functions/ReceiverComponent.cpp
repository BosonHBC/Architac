// Fill out your copyright notice in the Description page of Project Settings.


#include "Functions/ReceiverComponent.h"
#include "Functions/SenderComponent.h"
#include "GameFramework/Actor.h"
#include "WorldBuildingGrid.h"
#include "IslandMainProjectGameModeBase.h"
#include "Public/Building/Actors/BlockSet.h"

// Sets default values for this component's properties
UReceiverComponent::UReceiverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

bool UReceiverComponent::RegisterSender(USenderComponent* sender)
{
	if(IsValid(sender) && //If target is valid
		!IsValid(this->m_sender) && // If this receiver is not already registered to another sender
		sender->GetTransferPropertiesRow().RowName == this->TransferPropertiesDataTableRow.RowName)// If Transfer Properties Match
		
	{
		this->m_sender = sender;
		return true;
	}
	else
		return false;
}

bool UReceiverComponent::UnRegisterSender(USenderComponent* sender)
{
	if (this->m_sender == sender)
	{
		this->m_sender = nullptr;
		this->TryRegisterSendersInRange();
		return true;
	}
	else
		return false;
}

bool UReceiverComponent::ReceiveItems(USenderComponent* sender)
{
	if (!IsValid(sender) && sender != this->m_sender && !IsValid(this->m_receiverInventory))
		return false;

	return this->m_receiverInventory->AddToInventory(this->m_transferPropertiesData.Items, 1.0f);
}

void UReceiverComponent::SetReceiverProperties(UInventoryComponent* receiverInventory)
{
	if(IsValid(receiverInventory))
		this->m_receiverInventory = receiverInventory;
}

FDataTableRowHandle UReceiverComponent::GetTransferPropertiesRow() const
{
	return this->TransferPropertiesDataTableRow;
}

void UReceiverComponent::EnableBlockSetComponent()
{	
	Super::EnableBlockSetComponent();

	// Code Here
	this->TryRegisterSendersInRange();
}

void UReceiverComponent::DisableBlockSetComponent()
{
	Super::DisableBlockSetComponent();
}


// Called when the game starts
void UReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	if(TransferPropertiesDataTableRow.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Receiver Component Not Configured With Transfer Properties: %s"), *this->GetOwner()->GetName());
		return;
	}
	else
	{
		m_transferPropertiesData = *TransferPropertiesDataTableRow.GetRow<FTransferPropertiesRow>("");
	}
}

bool UReceiverComponent::TryRegisterSendersInRange()
{
	int range = m_transferPropertiesData.BlockIndexRange;

	AActor* owner = this->GetOwner();
	AIslandMainProjectGameModeBase* gameMode = Cast<AIslandMainProjectGameModeBase>(owner->GetWorld()->GetAuthGameMode());
	AWorldBuildingGrid * worldGrid = gameMode->GetWorldGridActor();

	ABlockSet* blockSetOwner = Cast<ABlockSet>(owner);
	TSet<ABlockSet*> blockSetsInRange = worldGrid->GetBuildingBlockSetInDistance(blockSetOwner->GetRootWorldIndex(), FIntVector(range, range, range));
	blockSetsInRange.Remove(blockSetOwner);

	// Find senders in range that can receive this receiver
	for (ABlockSet* blockSet : blockSetsInRange)
	{
		TArray<USenderComponent*> senders;
		blockSet->GetComponents<USenderComponent>(senders, true);

		if (senders.Num() > 0)
		{
			for (USenderComponent* sender : senders)
			{
				// Only accept senders that have a matching row
				if (sender->GetTransferPropertiesRow().RowName != this->TransferPropertiesDataTableRow.RowName)
					continue;
				
				bool success = sender->RegisterReceiverTarget(this);
				if (success)
				{
					RegisterSender(sender);
					return true;
				}
					
			}
		}

	}
	return false;
}
