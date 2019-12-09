// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockSetFunctionComponent.generated.h"


UENUM(BlueprintType, Category = "Block Set Functional Status")
enum class EComponentActiveStatus : uint8
{
	ENABLED UMETA(DisplayName = "Enabled"),
	DISABLED UMETA(DisplayName = "Disabled"),
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UBlockSetFunctionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlockSetFunctionComponent();

	UFUNCTION(BlueprintCallable)
	virtual void EnableBlockSetComponent();

	UFUNCTION(BlueprintCallable)
	virtual void DisableBlockSetComponent();

	UFUNCTION(BlueprintNativeEvent)
	void OnBlockSetFunctionEnable();

	UFUNCTION(BlueprintNativeEvent)
	void OnBlockSetFunctionDisable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	class ABlockSet* m_buildingSetParent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EComponentActiveStatus m_activeStatus = EComponentActiveStatus::DISABLED;

	
public:	
	void SetBuildingSetParent(class ABlockSet* i_pointer);
};
