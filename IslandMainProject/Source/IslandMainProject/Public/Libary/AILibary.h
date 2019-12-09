// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AILibary.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ETrailHeightLevel : uint8
{
	THL_DeepOcean  UMETA(DisplayName = "DeepOcean"),
	THL_ShallowOcean 	UMETA(DisplayName = "ShallowOcean"),
	THL_Air 	UMETA(DisplayName = "Air")
};

/**
 * Container for holding all AI enums, classes
 */
UCLASS()
class ISLANDMAINPROJECT_API UAILibary : public UObject
{
	GENERATED_BODY()
	
};
