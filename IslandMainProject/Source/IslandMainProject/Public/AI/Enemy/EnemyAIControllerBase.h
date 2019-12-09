// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Libary/AILibary.h"
#include "EnemyAIControllerBase.generated.h"

class ACircleTrail;
/**
 *
 */
UCLASS()
class ISLANDMAINPROJECT_API AEnemyAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIControllerBase();

protected:
	virtual void BeginPlay() override;

	void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

public:
	virtual void StopRunAILogic(const FString i_reason);
#pragma region Attack
public:
	/** If this enemy is going to attack player*/
	bool m_GoingToAttackTarget;
	AActor* GetCurrentTarget() const { return m_currentTargetActor; }
	virtual FVector GetCurrentTargetLocation() { return m_currentTargetActor->GetActorLocation(); }
	AActor* m_currentTargetActor;

	UFUNCTION(BlueprintCallable, Category = AI_Enemy)
		virtual void SetAttackTarget(AActor* i_target);
	/** Should be called when the target is destroy or other reasons*/
	UFUNCTION(BlueprintCallable, Category = AI_Enemy)
		virtual void CleanUpAttackRequest();
#pragma  endregion

#pragma region Trail
public:
	/** Virtual function for moving along circle trail*/
	virtual void MoveAlongCircle(const FVector& i_direction, float DeltaTime);

	virtual void MoveDirectlyTo(float DeltaTime);

	void SetUpMovingTrail(ACircleTrail* i_trail);

	/** if it is time to change trail, tell black board change trail*/
	bool IsTimeToChangeTrail() const;

	void ResetChangeTrailTimer();

	/** If the ai is close enough to the closest point*/
	bool IsCloesEnough() const;
	/** Get the current trail that this ai is in*/
	ACircleTrail* GetCircleTrail() const { return m_FollowingTrail; }

	ETrailHeightLevel GetFilteredHeightLevel() const { return FilteredTrailHeightLevel; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI_OceanEnemy)
		ETrailHeightLevel FilteredTrailHeightLevel;

	ACircleTrail* m_FollowingTrail;

	FVector m_cloestPoint;

	float m_timeSinceStart = 0;

	float m_collapseTime = 0;
	/** Base interval in sec of changing trail */
	float m_durationToChangeTrail;
#pragma endregion
};
