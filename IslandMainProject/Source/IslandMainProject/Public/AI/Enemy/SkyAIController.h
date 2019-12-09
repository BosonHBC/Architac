// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "SkyAIController.generated.h"

/**
 *
 */
UCLASS()
class ISLANDMAINPROJECT_API ASkyAIController : public AEnemyAIControllerBase
{
	GENERATED_BODY()
public:
	virtual void MoveAlongCircle(const FVector& i_direction, float DeltaTime) override;

	virtual FVector GetCurrentTargetLocation() override;

	/** If this bird can land on ground*/
	UFUNCTION(BlueprintCallable, Category = AI_Sky)
		bool IsLanded();
	/** This getter is to get the is landed bool, the previous one is to assign value for m_isLanded*/
	bool GetIsLanded() const { return m_isLanded; }
	virtual void StartLand();

protected:
	/** If this bird is walking on ground*/
	bool m_isLanded;
};
