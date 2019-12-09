// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockSetFunctionComponent.h"
#include "InventoryComponent.h"
#include "ReceiverComponent.generated.h"


class USenderComponent;
class UInventoryComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UReceiverComponent : public UBlockSetFunctionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UReceiverComponent();

	UFUNCTION()
	bool RegisterSender(USenderComponent* sender);
	UFUNCTION()
	bool UnRegisterSender(USenderComponent* sender);

	UFUNCTION()
	bool ReceiveItems(USenderComponent* sender);

	
	virtual void EnableBlockSetComponent() override;
	virtual void DisableBlockSetComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Receiver Properties")
	void SetReceiverProperties(UInventoryComponent* receiverInventory);

	UFUNCTION(BlueprintCallable, Category = "Receiver Accessor")
	FDataTableRowHandle GetTransferPropertiesRow() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	bool TryRegisterSendersInRange();
	
	UPROPERTY(VisibleInstanceOnly, Category = "Receiver Properties")
	USenderComponent * m_sender;
public:

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Receiver Properties")
	UInventoryComponent* m_receiverInventory; // Set Via Blueprint


	UPROPERTY(EditDefaultsOnly, Category = "Receiver Properties")
	FDataTableRowHandle TransferPropertiesDataTableRow;
	UPROPERTY()
	FTransferPropertiesRow m_transferPropertiesData;
};
