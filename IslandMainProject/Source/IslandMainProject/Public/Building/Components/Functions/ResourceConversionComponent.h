// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Map.h"
#include "BlockSetFunctionComponent.h"
#include "TimerManager.h"
#include "Public/InventoryComponent.h"
#include "ResourceConversionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UResourceConversionComponent : public UBlockSetFunctionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResourceConversionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Conversion Accessor")
	FConversionPropertiesRow GetConversionPropertiesData() const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ResourceConversion")
		void StartConvert();

	UFUNCTION(BlueprintCallable, Category = "ResourceConversion")
		bool IsInputResourceAvailable();

	/*UFUNCTION(BlueprintCallable, Category = "ResourceConversion")
		void GetInventoryItemByID(int datanumber);*/

	UFUNCTION(BlueprintCallable, Category = "ResourceConversion")
		void ConvertItem(int datanumber);

	UFUNCTION(BlueprintCallable, Category = "ResourceConversion")
		void OutputItem(int datanumber);


private:

	UPROPERTY(EditDefaultsOnly, Category = "Conversion Properties")
	FDataTableRowHandle ConversionPropertiesDataTableRow;
	UPROPERTY()
	FConversionPropertiesRow m_conversionPropertiesData;
	
	class UInventoryComponent* m_inventory;

	FTimerDelegate m_timerDel;
	FTimerHandle m_conversionTimeHandle;
};
