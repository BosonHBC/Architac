// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolBase.h"
#include "Damageable.h"
#include "ToolHarpoon.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API AToolHarpoon : public AToolBase
{
	GENERATED_BODY()
public:
	AToolHarpoon();

	void UpdateDamage(float Dmg);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly, Category = HarpoonTool)
		float Damage;

	// Parent functions
	void StartUseTool() override;
	void UseToolFinish() override;

	void GiveDamageToDamageable();

	bool m_canDealDmg = false;
	TMap<AActor*, FDamageInfo> m_damageableActorNbDamageDealtMap;
	FVector m_previousLocation;
	FVector m_currentLocation;
	UFUNCTION()
	virtual	void OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION()
	virtual	void OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


};
