// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/StaticLibrary.h"
#include "Public/InventoryComponent.h"


TArray<int> UStaticLibrary::GetAmountsOfRequiredResource(UDataTable* table, UInventoryComponent* inventory, FName sourceID)
{
	TArray<int> listOfAmount;
	if (!table || !inventory) return listOfAmount;

	// Find the source ID
	FItemAmountGroup* thisCostable = table->FindRow<FItemAmountGroup>(sourceID, "");
	if (thisCostable) {
		for (auto it = thisCostable->ItemAmounts.begin(); it != thisCostable->ItemAmounts.end(); ++it)
		{
			listOfAmount.Add(inventory->GetItemAmount((*it).ItemRow.RowName));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No data found in FItemAmountGroup Data table"));
	}
	return listOfAmount;
}

bool UStaticLibrary::CostResources(UDataTable* table, UInventoryComponent* inventory, FName sourceID)
{
	if (!table || !inventory) return false;

	// Find the source ID
	FItemAmountGroup* thisCostable = table->FindRow<FItemAmountGroup>(sourceID, "");
	if (thisCostable) {
		for (auto it = thisCostable->ItemAmounts.begin(); it != thisCostable->ItemAmounts.end(); ++it)
		{
			int removedAmount = inventory->ReduceItemAmount((*it).ItemRow.RowName, (*it).Amount);
			if (removedAmount < (*it).Amount) UE_LOG(LogTemp, Warning, TEXT("Query Amount is more than the amout player has, didn't check call [CheckIfResourceEnouth(FName)] before to make sure there is enough item in player's inventory"));
		}
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No data found in FItemAmountGroup Data table"));
	}
	return false;
}

bool UStaticLibrary::CheckIfResourceEnough(UDataTable* table, UInventoryComponent* inventory, FName sourceID)
{
	if (!table || !inventory) return false;

	// Find the source ID
	FItemAmountGroup* thisCostable = table->FindRow<FItemAmountGroup>(sourceID, "");
	if (thisCostable)
		return inventory->HasEnoughItems(*thisCostable);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No data found in FItemAmountGroup Data table"));
	}
	return false;
}
