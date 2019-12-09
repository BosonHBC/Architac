// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Public/StaticLibrary.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateHUD);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
		TArray<FInventoryItem> m_inventory;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Inventory")
		TArray<FInventoryItem> InitialItemList;
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FInventoryItem GetInventoryItem(int index);

	// add item interfaces return the remaining amount that is succesfully add to inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int AddItem(FName itemID, int amount);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int AddItemAtSlot(FName itemID, int amount, int slotID);

	// reduce item amount interfaces return the amount that is succesfully removed from inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int ReduceItemAmountAtSlot(FName itemID, int amount, int slotID);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int ReduceItemAmount(FName itemID, int amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveItem(FName itemID);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveItemAtSlot(int slotID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ThrowItem(FName itemID, int amount);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ThrowItemInDirection(FName itemID, int amount, FVector direction);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ThrowAllItems();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ThrowAllItemsInDirection(FVector direction);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ClearAllItems();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int GetInventorySlotNum();


#pragma region Query

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		FInventoryItem GetSlot(int slotID);

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool HasItem(FName itemID);
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool HasEnoughItem(const FItemAmount& costable);
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool HasEnoughItems(const FItemAmountGroup& costable);

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool RemoveFromInventory(const FItemAmountGroup& costable);
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		bool AddToInventory(const FItemAmountGroup& costable, float ratio);
	
	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		int GetItemAmount(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		FORCEINLINE TArray<FInventoryItem>& GetInventory() { return m_inventory; }

	UFUNCTION(BlueprintCallable, Category = "InventoryQuery")
		FORCEINLINE int GetNumOfSlots() { return m_slotCount; }


#pragma endregion
	bool IsExistingItem(FName itemID, FInventoryItem& t_item);

	void LoadInventoryData();

	void SpawnItemOutsideWorld(const FInventoryItem& itemInfo, int amount);

	void SpawnItemInDirection(const FInventoryItem& itemInfo, int amount, FVector direction);

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnUpdateHUD"))
		FOnUpdateHUD OnUpdateHUD;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		uint16 m_slotCount;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		int ThrowForce;

	void InitlizeInventory();

private:
	UDataTable* m_itemTable;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
