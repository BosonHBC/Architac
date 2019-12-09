// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Public/StaticLibrary.h"
#include "PickUpBase.h"
#include "Damageable.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGetKilled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDropItems, TSubclassOf<AActor>, item, FVector, loc);
class UBehaviorTree;
UCLASS()
class ISLANDMAINPROJECT_API AEnemyBase : public APawn, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, meta = (DisplayName = "Delegates"))
		FOnDropItems OnDropItems;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, meta = (DisplayName = "Delegates"))
		FOnGetKilled OnGetKilled;

	void InjectUniqueItem(TSubclassOf<APickUpBase> item);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class USkeletalMeshComponent* SuperMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UCapsuleComponent* CapComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UWidgetComponent* WidgetComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		class UInventoryComponent* LootComp;
	// NOTICE: If this pawn wants to move, it needs a PawnMovementComponent, which may be different depends on different types of enemy

	UPROPERTY()
		class TSubclassOf<APickUpBase> InjectedItem;

	void DropAllItems();

#pragma region BehaivourTree
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI)
		UBehaviorTree* m_BT;
public:
	UBehaviorTree* GetBehaviourTree() const { return m_BT; }

	/** Virtual function for both oceanEnemy and air enemy to move toward direction*/
	virtual void MoveTowardDirection(const FVector& i_direction, float DeltaTime);
	/** Set simulate physics for the cap component*/
	virtual void ToggleSimulatePhysics(bool i_enable);
#pragma endregion

# pragma region Damageable
protected:
	// Interface stuffs, only override get damage function and the die function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Damageable)
		void GetDamage(const FDamageInfo& dInfo);
	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnGetdamage(float ratioOfHealth);
	UFUNCTION(BlueprintCallable, Category = Damageable)
		virtual void Die();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Damageable)
		void BPDie();

	UFUNCTION(BlueprintCallable, Category = Damageable)
		void SetDamageCoolDown(bool i_isFinished);

	float m_knockbackScalar;
	FVector m_knockback;
	void HandleKnockBack();

	bool m_GetDamageCoolDownFinish = true;
# pragma endregion
#pragma region Attack
public:
	UPROPERTY(BlueprintReadWrite, Category = AI_Attack)
		bool IsAttacking;
	UFUNCTION(BlueprintCallable, Category = AI_Attack, meta = (AllowPrivateAccess = "true"))
		virtual void FinishAttack() {};

	virtual void PerformAttack() {};
#pragma endregion

};
