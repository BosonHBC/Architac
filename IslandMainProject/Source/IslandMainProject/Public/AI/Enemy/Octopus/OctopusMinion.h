// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/OceanEnemy.h"
#include "OctopusMinion.generated.h"

/**
 *
 */
UCLASS()
class ISLANDMAINPROJECT_API AOctopusMinion : public AOceanEnemy
{
	GENERATED_BODY()

public:
	AOctopusMinion();
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI_Attack)
		float DamageDealt;

	UFUNCTION()
		virtual	void OnDamageableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	virtual void Die() override;

public:
	UFUNCTION(BlueprintCallable, Category = AI_Attack, meta = (AllowPrivateAccess = "true"))
		void ToggleAttackPoint(bool enable);

	UFUNCTION(BlueprintImplementableEvent, Category = AI_Attack)
		void BPPerformAttack();

	void PerformAttack() override;

	//UFUNCTION(BlueprintCallable, Category = AI_Attack, meta = (AllowPrivateAccess = "true"))
		void FinishAttack() override;
#pragma endregion

};
