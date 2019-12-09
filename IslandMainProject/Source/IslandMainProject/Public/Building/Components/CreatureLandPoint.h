// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "CreatureLandPoint.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (LandPoint), Blueprintable, meta = (BlueprintSpawnableComponent))
class ISLANDMAINPROJECT_API UCreatureLandPoint : public USphereComponent
{
	GENERATED_BODY()
public:
	UCreatureLandPoint();

};
