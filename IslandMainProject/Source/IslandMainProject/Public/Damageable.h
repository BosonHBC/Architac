// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef KNOCKBACK_SCALAR
#define KNOCKBACK_SCALAR 3
#endif

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EDamageSource : uint8
{
	EDS_GameMaster		UMETA(DisplayName = "GameMaster"),
	EDS_Player		UMETA(DisplayName = "Player"),
	EDS_Enemy 	UMETA(DisplayName = "Enemy"),
	EDS_Envrionment 	UMETA(DisplayName = "Environment")
};

USTRUCT(BlueprintType)
struct FDamageInfo {
	GENERATED_USTRUCT_BODY()
public:
	FDamageInfo() {
		Damage = 0;
		DamageDirection = FVector::ZeroVector;
		DamagePoint = FVector::ZeroVector;
		DamageSource = EDamageSource::EDS_GameMaster;
		bDamaged = false;
	}

	int Damage;
	FVector DamageDirection;
	FVector DamagePoint;
	EDamageSource DamageSource;
	bool bDamaged;
};

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ISLANDMAINPROJECT_API IDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Damageable)
	void GetDamage(const FDamageInfo& dInfo);
	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnGetdamage(float ratioOfHealth);

	/** Current health can max health*/
	int m_currentHealth;
	int m_maxHealth;

	void Die();

	/** This mask Determine this damageable can NOT be damaged by which type of damage source*/
	TArray<EDamageSource> m_DamageableMask;
	/** Determine if this damageable can be damaged right now*/
	bool m_CanGetDamage = true;

protected:
};
