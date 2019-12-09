// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "BlockSetFunctionComponent.h"
#include "StaticLibrary.h"
#include "ResourceGenerationComponent.generated.h"

UENUM(BlueprintType)
enum class EFactoryState : uint8
{
	E_ISFULL    UMETA(DisplayName = "Factory Status"),
	E_ISNOTFULL UMETA(DisplayName = "Factory Status"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UResourceGenerationComponent : public UBlockSetFunctionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResourceGenerationComponent();

	EFactoryState factorystate = EFactoryState::E_ISNOTFULL;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EnableBlockSetComponent() override;
	virtual void DisableBlockSetComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	class UInventoryComponent* m_inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Generation Properties")
	FDataTableRowHandle GenerationPropertiesDataTableRow;
	UPROPERTY()
	FGenerationPropertiesRow m_generationPropertiesRow;
	
	float m_elapsedTime = 0.0f;
};
