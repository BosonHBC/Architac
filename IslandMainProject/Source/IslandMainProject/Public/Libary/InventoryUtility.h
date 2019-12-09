// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryUtility.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UInventoryUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		static void TransferItem(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		static void TransferItemOnlyBySrcSlot(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount, int srcSlotID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		static void TransferItemOnlyByDstSlot(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount, int dstSlotID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		static void TransferItemBySlot(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory, FName itemID, int amount, int srcSlotID, int dstSlotID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		static void MoveInventoryToAnother(class UInventoryComponent* SrcInventroy, class UInventoryComponent* DstInventory);
};
