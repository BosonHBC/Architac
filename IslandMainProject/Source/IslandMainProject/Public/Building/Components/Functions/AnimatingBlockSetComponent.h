// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Components/BlockSetFunctionComponent.h"
#include "AnimatingBlockSetComponent.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UAnimatingBlockSetComponent : public UBlockSetFunctionComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UAnimatingBlockSetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animating Properties", meta = (AllowPrivateAccess = "true"))
		TArray<USkeletalMeshComponent*> m_skeletalMeshesToEnableAndDisable;
};

