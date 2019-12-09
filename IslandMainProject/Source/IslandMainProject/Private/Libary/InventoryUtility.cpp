// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUtility.h"
#include "Public/InventoryComponent.h"


void UInventoryUtility::TransferItem(UInventoryComponent* SrcInventroy, UInventoryComponent* DstInventory, FName itemID, int amount)
{
	TransferItemBySlot(SrcInventroy, DstInventory, itemID, amount, -1, -1);
}

void UInventoryUtility::MoveInventoryToAnother(UInventoryComponent* SrcInventroy, UInventoryComponent* DstInventory)
{
	for (auto it = SrcInventroy->m_inventory.begin(); it != SrcInventroy->m_inventory.end(); ++it)
	{
		TransferItem(SrcInventroy, DstInventory, (*it).ItemID, (*it).Amount);
	}
	
}

void UInventoryUtility::TransferItemBySlot(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount, int srcSlotID, int dstSlotID)
{
	int needAddNum = amount;
	int realAddNum = 0;
	// check the number of specific item src inventory has
	// and update the real add number of ceratin item
	if (SrcInventroy)
	{
		if (srcSlotID == -1)
		{
			needAddNum = FMath::Min(SrcInventroy->GetItemAmount(itemID), needAddNum);
		}
		else
		{
			auto srcSlot = SrcInventroy->GetSlot(srcSlotID);
			if (srcSlot.ItemID == itemID)
			{
				needAddNum = FMath::Min(srcSlot.Amount, needAddNum);
			}
			else
			{
				needAddNum = 0;
			}
		}
	}
	// the destinational inventory has its limitation
	// get the actual added num to the destination
	if (DstInventory)
	{
		if (dstSlotID == -1)
		{
			realAddNum = DstInventory->AddItem(itemID, needAddNum);
		}
		else
		{
			realAddNum = DstInventory->AddItemAtSlot(itemID, needAddNum, dstSlotID);
		}
	}

	// reduce the number adding to the destination from the source inventory
	if (SrcInventroy)
	{
		if (srcSlotID == -1)
		{
			SrcInventroy->ReduceItemAmount(itemID, realAddNum);
		}
		else
		{
			SrcInventroy->ReduceItemAmountAtSlot(itemID, realAddNum, srcSlotID);
		}
	}

}

void UInventoryUtility::TransferItemOnlyBySrcSlot(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount, int srcSlotID)
{
	TransferItemBySlot(SrcInventroy, DstInventory, itemID, amount, srcSlotID, -1);
}

void UInventoryUtility::TransferItemOnlyByDstSlot(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount, int dstSlotID)
{
	TransferItemBySlot(SrcInventroy, DstInventory, itemID, amount, -1, dstSlotID);
}