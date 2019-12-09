// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockSetFunctionComponent.h"
#include "Engine/DataTable.h"
#include "StaticLibrary.h"
#include "ConsumptionComponent.generated.h"

// Component that consumes resources to enable the block set 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UConsumptionComponent : public UBlockSetFunctionComponent 
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UConsumptionComponent();

	UFUNCTION(BlueprintCallable, Category = "Sender Properties")
	void SetConsumptionProperties(UInventoryComponent* consumptionInventory);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Sender Properties")
	UInventoryComponent* m_consumptionInventory; // Set Via Blueprint
	
	UPROPERTY(EditDefaultsOnly, Category = "Consumption Properties")
	FDataTableRowHandle ConsumptionPropertiesDataTableRow;
	UPROPERTY()
	FConsumptionPropertiesRow m_consumptionPropertiesData;

	float m_elapsedTime = 0.0f;
};
