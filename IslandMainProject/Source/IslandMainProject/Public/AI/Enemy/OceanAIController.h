// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "OceanAIController.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API AOceanAIController : public AEnemyAIControllerBase
{
	GENERATED_BODY()
public:
	AOceanAIController();

	virtual void MoveAlongCircle(const FVector& i_direction, float DeltaTime) override;

	virtual FVector GetCurrentTargetLocation() override;
};
