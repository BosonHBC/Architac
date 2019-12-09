// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/SkyEnemy.h"
#include "FatBird.generated.h"

/**
 *
 */
UCLASS()
class ISLANDMAINPROJECT_API AFatBird : public ASkyEnemy
{
	GENERATED_BODY()

public:
	AFatBird();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class USphereComponent* AttackPoint;
	// OctopusMinion should use floating pawn movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UFloatingPawnMovement* MoveComp;

#pragma region AI
protected:
	
	/** Handles horizontally velocity for animation*/
	// ------------------------------------------------------------------------
	FVector2D m_previousLocation;
	FVector2D m_currentLocation;
	float m_horizontalVelocitySqr;

	UFUNCTION(BlueprintCallable, Category = AI_Sky)
		float GetHorizontalVelocity() const { return m_horizontalVelocitySqr; }

protected:
	
	/** Handle Enemy Attack*/
	// ------------------------------------------------------------------------
	// Which Direction is the target is, on the bottom or not, if it is on the bottom, use bottom attack, if not, use front attack
	UFUNCTION(BlueprintCallable, Category = AI_Attack)
		bool IsTargetOnBottom();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI_Attack)
		float DamageDealt;
	UFUNCTION()
		virtual	void OnDamageableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable, Category = AI_Attack, meta = (AllowPrivateAccess = "true"))
		void ToggleAttackPoint(bool enable);

	UFUNCTION(BlueprintImplementableEvent, Category = AI_Attack)
		void BPPerformAttack();

	void PerformAttack() override;

	void FinishAttack() override;

#pragma endregion
};
