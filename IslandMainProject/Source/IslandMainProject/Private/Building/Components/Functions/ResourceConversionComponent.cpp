// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceConversionComponent.h"
#include "BlockSet.h"
#include "IslandMainProjectGameModeBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "VenturePawn.h"
#include "IslandMainProjectGameModeBase.h"
#include "Public/InventoryComponent.h"

// Sets default values for this component's properties
UResourceConversionComponent::UResourceConversionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UResourceConversionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get Properties Row
	if (ConversionPropertiesDataTableRow.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Conversion Component Not Configured With Conversion Properties: %s"), *this->GetOwner()->GetName());
		return;
	}
	else
	{
		m_conversionPropertiesData = *ConversionPropertiesDataTableRow.GetRow<FConversionPropertiesRow>("");
	}

	
	// Set Inventory Target To Player
	if (m_buildingSetParent)
	{
		AIslandMainProjectGameModeBase* gamemode = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
		AVenturePawn *  venture = gamemode->GetVenturePawn();
		if (venture != nullptr)
		{			
			m_inventory = venture->GetInventoryComponent();
		}
		
	}

	// ...
	
}

FConversionPropertiesRow UResourceConversionComponent::GetConversionPropertiesData() const
{
	return this->m_conversionPropertiesData;
}


// Called every frame
void UResourceConversionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UResourceConversionComponent::IsInputResourceAvailable()
{
	if (m_inventory)
	{

		// For each resource that needs to be converted
		for (auto it = this->m_conversionPropertiesData.InputItems.ItemAmounts.begin(); it != this->m_conversionPropertiesData.InputItems.ItemAmounts.end(); ++it)
		{
			// Confirm there is enough to convert
			if (m_inventory->HasItem((*it).ItemRow.RowName))
			{
				if (!(m_inventory->GetItemAmount((*it).ItemRow.RowName) >= (*it).Amount))
				{
					UE_LOG(LogTemp, Log, TEXT("There are not enough amount of the item in inventory component"));
					return false;
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("There is no such items in inventory component"));
				return false;
			}
		}
		return true;
	}
	UE_LOG(LogTemp, Log, TEXT("Something went wrong with IsInputResourceAvaibale"));
	return false;
}

//void UResourceConversionComponent::GetInventoryItemByID(int datanumber)
//{
//	AActor* owner = this->GetOwner();
//	UWorld* world = owner->GetWorld();
//	auto GM = Cast <AIslandMainProjectGameModeBase>(world->GetAuthGameMode());
//	UDataTable* itemtable;
//	if (GM)
//	{
//		itemtable = GM->GetItemDB();
//
//		if (itemtable)
//		{
//			
//			FInventoryItem* firstItem = this->m_conversionPropertiesData.InputItems.ItemAmounts[0].ItemRow.GetRow<FInventoryItem>("");
//
//			
//			if (firstItem != nullptr)
//			{
//				DataList[datanumber].InputItems[0].Item.Thumbnail = item->Thumbnail;
//			}
//
//			item = itemtable->FindRow<FInventoryItem>(DataList[datanumber].OutputItems[0].Item.ItemID, "");
//			if (item != nullptr)
//			{
//				DataList[datanumber].OutputItems[0].Item.Thumbnail = item->Thumbnail;
//			}
//		}
//	}
//}

void UResourceConversionComponent::ConvertItem(int datanumber)
{
	if (m_inventory)
	{
		if (IsInputResourceAvailable())
		{
			// Reduce input items
			for (auto it = this->m_conversionPropertiesData.InputItems.ItemAmounts.begin(); it != this->m_conversionPropertiesData.InputItems.ItemAmounts.end(); ++it)
			{
				if ((*it).Amount > 0)
				{
					m_inventory->ReduceItemAmount((*it).ItemRow.RowName, (*it).Amount);
				}
			}
			
			// To Do : need to fixed this to check what's is the limited amount of items the inventory can hold and how many items does inventory has
			// Check if there is enough slot to add converted item, if not revert everything.
			if (m_inventory->GetInventory().Num() + this->m_conversionPropertiesData.OutputItems.ItemAmounts.Num() > 1000)
			{
				for (auto it = this->m_conversionPropertiesData.InputItems.ItemAmounts.begin(); it != this->m_conversionPropertiesData.InputItems.ItemAmounts.end(); ++it)
				{
					m_inventory->AddItem((*it).ItemRow.RowName, (*it).Amount);
				}
				UE_LOG(LogTemp, Log, TEXT("There is not enough slots to put output items in inventory component."));
				return;
			}
			else
			{
				m_timerDel.BindUFunction(this, FName("OutputItem"), datanumber);
				GetOwner()->GetWorldTimerManager().SetTimer(m_conversionTimeHandle, m_timerDel, this->m_conversionPropertiesData.RateInSeconds, false, this->m_conversionPropertiesData.RateInSeconds);
				m_buildingSetParent->CanProcessNextAction = false;
			}
		}
	}
}

void UResourceConversionComponent::OutputItem(int datanumber)
{
	for (auto it = this->m_conversionPropertiesData.OutputItems.ItemAmounts.begin(); it != this->m_conversionPropertiesData.OutputItems.ItemAmounts.end(); ++it)
	{
		if ((*it).Amount > 0)
		{
			m_inventory->AddItem((*it).ItemRow.RowName, (*it).Amount);
		}
	}
	m_buildingSetParent->CanProcessNextAction = true;
}

