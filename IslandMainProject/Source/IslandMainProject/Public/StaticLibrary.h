// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

#include "StaticLibrary.generated.h"


class UInventoryComponent;
USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase {

	GENERATED_USTRUCT_BODY()

public:
	FInventoryItem() {
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Describtion = FText::FromString("Add Description");
		Description = FText::FromString("Add Description");
		Amount = 0;
		MaxStack = 10000;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class APickUpBase> ItemPickUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* Thumbnail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Describtion;// TODO need to move data over as a result of a spelling mistake
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanBeUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanStack;

	void ClearItem() {
		ItemID = FName(TEXT("None"));
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Describtion = FText::FromString("Add Description");
		Thumbnail = nullptr;
		Amount = 0;
	}
	int AddItem(int i_v)
	{
		int exceed = 0;
		Amount += i_v;
		if (Amount > MaxStack)
		{
			exceed = Amount - MaxStack;
			Amount = MaxStack;
		}
		return exceed;
	}
	int RemoveItem(int i_v)
	{
		int exceed = 0;
		Amount -= i_v;
		if (Amount <= 0)
		{
			exceed = -Amount;
			ClearItem();
		}
		return exceed;
	}

	bool IsNone() { return ItemID.IsEqual(FName(TEXT("None"))); }

	bool operator==(const FInventoryItem& other) const {
		return ItemID == other.ItemID;
	}

	static FInventoryItem EmptySlotItem() { FInventoryItem v; v.ClearItem(); return v; }
};


USTRUCT(BlueprintType)
struct FItemAmount{
	GENERATED_USTRUCT_BODY()
public:
	FItemAmount() {
		Amount = 0;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ItemRow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int   MaxStack;
};


USTRUCT(BlueprintType)
struct FItemAmountGroup : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FItemAmountGroup() {
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FItemAmount> ItemAmounts;
};


USTRUCT(BlueprintType)
struct FConversionPropertiesRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	FConversionPropertiesRow()
	{
		
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemAmountGroup InputItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemAmountGroup OutputItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RateInSeconds;
};

USTRUCT(BlueprintType)
struct FBuildingIndicationRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	FBuildingIndicationRow(){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D * texturetype1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D * texturetype2;
};

USTRUCT(BlueprintType)
struct FTransferPropertiesRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	FTransferPropertiesRow()
	{
		RateInSeconds = 1;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemAmountGroup Items;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RateInSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BlockIndexRange;
};

USTRUCT(BlueprintType)
struct FGenerationPropertiesRow: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	FGenerationPropertiesRow(){
		
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemAmountGroup Items;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RateInSeconds;
};

USTRUCT(BlueprintType)
struct FConsumptionPropertiesRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
		FConsumptionPropertiesRow() {

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FItemAmountGroup Items;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RateInSeconds;
};


USTRUCT(BlueprintType)
struct FSoundEffectRow : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	FSoundEffectRow() {
		Channel = "Default";
		Sound = nullptr;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundBase* Sound;
};

/**
 * A class to hold static my functions
 */
UCLASS()
class ISLANDMAINPROJECT_API UStaticLibrary : public UObject
{
	GENERATED_BODY()
	
		// Add more static functions here later on
public:
	UFUNCTION(BlueprintCallable, Category = "Building System")
		static TArray<int> GetAmountsOfRequiredResource(UDataTable* costTable, UInventoryComponent* inventory, FName sourceID);

	UFUNCTION(BlueprintCallable, Category = "Building System")
		static bool CostResources(UDataTable* costTable, UInventoryComponent* inventory, FName sourceID);

	UFUNCTION(BlueprintCallable, Category = "Builiding System")
		static bool CheckIfResourceEnough(UDataTable* costTable, UInventoryComponent* inventory, FName sourceID);

};
