// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/EnemyBase.h"
#include "OceanEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API AOceanEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
		AOceanEnemy();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI_OceanCreature)
		float SwimSpeed;

public:
	virtual void MoveTowardDirection(const FVector& i_direction, float DeltaTime) override;

protected:
	virtual void Die() override;
};
