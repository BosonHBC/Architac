// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockSetFunctionComponent.h"
#include "BuildingLibrary.h"
#include "SenderComponent.generated.h"

class UInventoryComponent;
class UReceiverComponent;

// Sender component possesses a one to many relationship with receivers
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API USenderComponent : public UBlockSetFunctionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USenderComponent();


	UFUNCTION()
	bool RegisterReceiverTarget(UReceiverComponent* target);
	UFUNCTION()
	bool UnRegisterReceiverTarget(UReceiverComponent* target);
	UFUNCTION()
	void NotifyReceiveActors(ESenderReceiverMessageType type);
	
	virtual void EnableBlockSetComponent() override;
	virtual void DisableBlockSetComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Sender Properties")
	void SetSenderProperties(UInventoryComponent* senderInventory);

	UFUNCTION(BlueprintCallable, Category = "Sender Accessor")
	FDataTableRowHandle GetTransferPropertiesRow() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
	UPROPERTY(VisibleInstanceOnly, Category = "Sender Properties")
	TSet<UReceiverComponent*> m_registeredReceivers;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(VisibleInstanceOnly, Category = "Sender Properties")
	UInventoryComponent* m_senderInventory; // Set Via Blueprint
	
	UPROPERTY(EditDefaultsOnly, Category = "Sender Properties")
	FDataTableRowHandle TransferPropertiesDataTableRow;
	UPROPERTY()
	FTransferPropertiesRow m_transferPropertiesData;

	UPROPERTY()
	float m_elapsedTime = 0.0f;
};
