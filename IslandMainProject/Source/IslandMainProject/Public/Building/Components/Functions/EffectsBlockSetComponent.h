// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Components/BlockSetFunctionComponent.h"
#include "EffectsBlockSetComponent.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UEffectsBlockSetComponent : public UBlockSetFunctionComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEffectsBlockSetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
