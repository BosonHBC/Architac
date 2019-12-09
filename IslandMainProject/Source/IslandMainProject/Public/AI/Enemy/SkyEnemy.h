// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/EnemyBase.h"
#include "SkyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API ASkyEnemy : public AEnemyBase
{
	GENERATED_BODY()
public:
	ASkyEnemy();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI_SkyCreature)
		float FlyingSpeed;

public:
	virtual void MoveTowardDirection(const FVector& i_direction, float DeltaTime) override;

	/** If this bird can land on ground*/
	virtual bool CheckIsLanded();
protected:
	FCollisionQueryParams m_collisionParam;

	virtual void Die() override;
};
