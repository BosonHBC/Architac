// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/InventoryComponent.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/World.h"
#include "Public/PickUpBase.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_slotCount = 10;
	ThrowForce = 150;

}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	auto GM = Cast <AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		m_itemTable = GM->GetItemDB();
	}
	m_inventory.Init(FInventoryItem::EmptySlotItem(), m_slotCount);
	InitlizeInventory();
	// ...

}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::InitlizeInventory()
{
	// Initialize a certain size of list
	//LoadInventoryData(); 
	for (auto it = InitialItemList.begin(); it != InitialItemList.end(); ++it)
	{
		AddItem((*it).ItemID, (*it).Amount);
	}
}

bool UInventoryComponent::IsExistingItem(FName itemID, FInventoryItem &t_item)
{
	if (m_itemTable) {
		FInventoryItem* ItemToAdd = m_itemTable->FindRow<FInventoryItem>(itemID, "");
		if (ItemToAdd == nullptr)
		{
			return false;
		}
		t_item = *ItemToAdd;
		return true;
	}
	return false;
}

FInventoryItem UInventoryComponent::GetInventoryItem(int index)
{
	return m_inventory[index];
}

int UInventoryComponent::AddItemAtSlot(FName itemID, int amount, int slotID)
{
	FInventoryItem tmp;
	if (!IsExistingItem(itemID, tmp))
	{
		return 0;
	}
	if (slotID < 0 || m_inventory.Num() <= slotID || (m_inventory[slotID].ItemID != itemID && m_inventory[slotID].ItemID != FName(TEXT("None"))))
	{
		return 0;
	}
	int remainingAddNum = 0;
	m_inventory[slotID].ItemID = itemID;
	remainingAddNum = m_inventory[slotID].AddItem(amount);
	OnUpdateHUD.Broadcast();
	return amount - remainingAddNum;
}

int UInventoryComponent::AddItem(FName itemID, int amount)
{
	int remainingAddNum = amount;
	FInventoryItem currentItem;
	if (!IsExistingItem(itemID, currentItem))
	{
		return 0;
	}

	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		// Check if it is existing item
		if ((*it).ItemID.IsEqual(itemID) && (*it).bCanStack) {			
			remainingAddNum = (*it).AddItem(remainingAddNum);
			if (remainingAddNum <= 0)
			{
				break;
			}
		}
	}
	if (remainingAddNum > 0)
	{
		currentItem.Amount = 0;

		for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
		{
			if ((*it).ItemID.IsEqual(FName("None"))) {
				
				(*it) = currentItem;
				remainingAddNum = (*it).AddItem(remainingAddNum);
				if (remainingAddNum <= 0)
				{
					break;
				}
			}
		}
	}

	OnUpdateHUD.Broadcast();
	return amount - remainingAddNum;
}

int UInventoryComponent::ReduceItemAmountAtSlot(FName itemID, int amount, int slotID)
{
	int remainingReduceNum = amount;
	if (slotID < 0 || m_inventory.Num() <= slotID)
	{
		return 0;
	}
	if (m_inventory[slotID].ItemID == itemID)
	{
		remainingReduceNum = m_inventory[slotID].RemoveItem(m_inventory[slotID].Amount);
	}
	else
	{
		return 0;
	}

	OnUpdateHUD.Broadcast();
	return amount - remainingReduceNum;

}


int UInventoryComponent::ReduceItemAmount(FName itemID, int amount)
{
	//UE_LOG(LogTemp, Log, TEXT("Reduce %s Amount: %d"), *itemID.ToString(), amount);
	int remainingReduceNum = amount;

	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		// Check if it is existing item
		if ((*it).ItemID.IsEqual(itemID)) {
			
			remainingReduceNum = (*it).RemoveItem(remainingReduceNum);
			if (remainingReduceNum <= 0)
			{
				break;
			}
		}
	}

	OnUpdateHUD.Broadcast();
	return amount - remainingReduceNum;
}

void UInventoryComponent::RemoveItemAtSlot(int slotID)
{
	if (slotID < 0 || m_inventory.Num() <= slotID)
	{
		return;
	}
	m_inventory[slotID].ClearItem();
	OnUpdateHUD.Broadcast();
}

void UInventoryComponent::RemoveItem(FName itemID)
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID.IsEqual(itemID)) {
			(*it).ClearItem();
		}
	}
	OnUpdateHUD.Broadcast();
}

void UInventoryComponent::ThrowItem(FName itemID, int amount)
{
	FInventoryItem currentItem;
	if (IsExistingItem(itemID, currentItem)) {
		int realRecudeAmount = amount;
		if (realRecudeAmount != 0) {
			FInventoryItem reducedItem;
			realRecudeAmount = ReduceItemAmount(itemID, amount);
			if (realRecudeAmount > 0)
			{
				SpawnItemOutsideWorld(currentItem, realRecudeAmount);
			}
		}
	}
}

void UInventoryComponent::ThrowItemInDirection(FName itemID, int amount, FVector direction)
{
	FInventoryItem currentItem;
	if (IsExistingItem(itemID, currentItem))
	{
		int realRecudeAmount = amount;
		if (realRecudeAmount != 0)
		{
			FInventoryItem reducedItem;
			realRecudeAmount = ReduceItemAmount(itemID, amount);
			if (realRecudeAmount > 0)
			{
				SpawnItemInDirection(currentItem, realRecudeAmount, direction);
			}
		}
	}
}


void UInventoryComponent::ThrowAllItems()
{
	// TODO
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID != "None")
		{
			SpawnItemOutsideWorld(*it, (*it).Amount);
		}
	}
	ClearAllItems();
}

void UInventoryComponent::ThrowAllItemsInDirection(FVector direction)
{
	// TODO
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID != "None")
		{
			SpawnItemInDirection(*it, (*it).Amount, direction);
		}
	}
	ClearAllItems();
}

void UInventoryComponent::SpawnItemOutsideWorld(const FInventoryItem& itemInfo, int amount)
{
	TSubclassOf<APickUpBase> spawnedClass = itemInfo.ItemPickUp;
	if (spawnedClass) {
		FActorSpawnParameters spawnPara;
		spawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APickUpBase* spawnActor = GetWorld()->SpawnActor<APickUpBase>(spawnedClass, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, spawnPara);
		spawnActor->AddImpulseToOverlapComp(GetOwner()->GetActorUpVector() + FMath::RandRange(-1, 1) * GetOwner()->GetActorForwardVector() + FMath::RandRange(-1, 1) * GetOwner()->GetActorRightVector(), ThrowForce);
		spawnActor->Amount = amount;

	}
}

void UInventoryComponent::SpawnItemInDirection(const FInventoryItem& itemInfo, int amount, FVector direction)
{
	TSubclassOf<APickUpBase> spawnedClass = itemInfo.ItemPickUp;
	if (spawnedClass)
	{
		FActorSpawnParameters spawnPara;
		spawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		APickUpBase* spawnActor = GetWorld()->SpawnActor<APickUpBase>(spawnedClass, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, spawnPara);
		direction.Normalize();
		spawnActor->AddImpulseToOverlapComp(GetOwner()->GetActorUpVector() + direction, ThrowForce);
		spawnActor->Amount = amount;

	}
}

void UInventoryComponent::ClearAllItems()
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		(*it).ClearItem();
	}

	OnUpdateHUD.Broadcast();
}

int UInventoryComponent::GetInventorySlotNum()
{
	return m_slotCount;
}


void UInventoryComponent::LoadInventoryData()
{
	//... for future loading data
}

bool UInventoryComponent::HasItem(FName itemID)
{
	for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
	{
		if ((*it).ItemID.IsEqual(itemID)) {
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasEnoughItem(const FItemAmount& costable)
{
	int requireNum = costable.Amount;
	int currentAmount = 0;
	
	for (auto it2 = m_inventory.begin(); it2 != m_inventory.end(); ++it2)
	{
		FName costName = costable.ItemRow.RowName;
		FName itemName = (*it2).ItemID;
		if (costName == itemName) {
			currentAmount += (*it2).Amount;
			if (currentAmount >= requireNum)
				return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasEnoughItems(const FItemAmountGroup& costable)
{
	auto costPairList = costable.ItemAmounts;

	// For each cost, check if in inventory
	for (auto it = costPairList.begin(); it != costPairList.end(); ++it)
	{
		// For each item in inventory, find a match, and check if they have enough
		// Lacking one item means you can exit early with false.
		if (!HasEnoughItem(*it)) 
			return false;
	}
	return true;
}

bool UInventoryComponent::RemoveFromInventory(const FItemAmountGroup& costable)
{
	auto costPairList = costable.ItemAmounts;

	// TODO Currently just returns true. Need to change ReduceItemAmount interface to a success fail interface instead
	// For each cost, check if in inventory
	for (auto it = costPairList.begin(); it != costPairList.end(); ++it)
	{
		this->ReduceItemAmount((*it).ItemRow.RowName, (*it).Amount);
	}
	return true;
}

bool UInventoryComponent::AddToInventory(const FItemAmountGroup& costable, float ratio)
{
	auto costPairList = costable.ItemAmounts;

	// TODO Currently just returns true. Need to change ReduceItemAmount interface to a success fail interface instead
	// For each cost, check if in inventory
	for (auto it = costPairList.begin(); it != costPairList.end(); ++it)
	{
		bool success = this->AddItem((*it).ItemRow.RowName, UKismetMathLibrary::FCeil(static_cast<float>((*it).Amount) * ratio));
	}
	return true;
}

int UInventoryComponent::GetItemAmount(FName ItemID)
{
	FInventoryItem currentItem;
	if (IsExistingItem(ItemID, currentItem)) {
		int amount = 0;
		for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
		{
			if ((*it).ItemID == ItemID) {
				amount += (*it).Amount;
			}
		}
		return amount;
	}
	return 0;
}

FInventoryItem UInventoryComponent::GetSlot(int slotID)
{
	if (slotID < 0 || slotID >= m_inventory.Num())
	{
		return FInventoryItem::EmptySlotItem();
	}
	return m_inventory[slotID];
}