// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/AI/Enemy/EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Public/InventoryComponent.h"
// AI
#include "TimerManager.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/PawnMovementComponent.h"

#include "DrawDebugHelpers.h"

#include "Libary\StaticFunctionLibary.h"


#define KnockBackFallOffScalarPerFrame 0.75f
#define KnockBackDisappearThreshold_Sqr 100
#define KnockBackScalar 100000

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapComp"));
	RootComponent = CapComp;
	CapComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SuperMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetupAttachment(RootComponent);
	SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusBar"));
	WidgetComp->SetupAttachment(RootComponent);

	LootComp = CreateDefaultSubobject <UInventoryComponent>(TEXT("LootComp"));

	{
		m_CanGetDamage = true;
		m_GetDamageCoolDownFinish = true;
		m_DamageableMask.Add(EDamageSource::EDS_Enemy);
		m_knockbackScalar = 1;
	}

}

void AEnemyBase::InjectUniqueItem(TSubclassOf<APickUpBase> item)
{
	InjectedItem = item;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleKnockBack();
}

void AEnemyBase::DropAllItems()
{
	LootComp->ThrowAllItems();

	if (IsValid(InjectedItem))
	{
		if (InjectedItem->IsChildOf((AActor::StaticClass())))
		{
			TSubclassOf<AActor> actor = *InjectedItem;
			OnDropItems.Broadcast(actor, this->GetActorLocation());
		}
	}
	OnGetKilled.Broadcast();
}




void AEnemyBase::GetDamage_Implementation(const FDamageInfo& dInfo)
{
	if (!m_GetDamageCoolDownFinish || !m_CanGetDamage || m_DamageableMask.Contains(dInfo.DamageSource)) return;

	//Basic reduce health
	float actualDmg;
	if (dInfo.Damage > m_currentHealth)
		actualDmg = m_currentHealth;
	else actualDmg = dInfo.Damage;
	m_currentHealth -= actualDmg;
	FVector dir = dInfo.DamageDirection.GetSafeNormal();

	// Knock back
	m_knockback = dInfo.DamageDirection * KnockBackScalar;

	m_GetDamageCoolDownFinish = false;
	BPOnGetdamage(m_currentHealth / (float)m_maxHealth);
	if (m_currentHealth <= 0)
		Die();
}

void AEnemyBase::MoveTowardDirection(const FVector& i_direction, float DeltaTime)
{
	// Parent implementation
	// ...
}

void AEnemyBase::ToggleSimulatePhysics(bool i_enable)
{
	CapComp->SetSimulatePhysics(i_enable);
}

void AEnemyBase::Die()
{
	// spawn loot objects
	DropAllItems();

	CapComp->SetSimulatePhysics(true);
	CapComp->SetEnableGravity(true);
	CapComp->SetConstraintMode(EDOFMode::None);

	// Stop Behavior tree
	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(GetController());
	aiCtrl->StopRunAILogic("I AM DEAD");

	// Delay To Destroy
	BPDie();

}

void AEnemyBase::SetDamageCoolDown(bool i_isFinished)
{
	m_GetDamageCoolDownFinish = i_isFinished;
	UE_LOG(LogTemp, Log, TEXT("Bird Get Dmg Cool Down Finish"));
}

void AEnemyBase::HandleKnockBack()
{

	// If it is getting damage
	if (!m_GetDamageCoolDownFinish) {
		// apply knock back vector
		AddMovementInput(m_knockback, m_knockbackScalar);
		// knock back fall off
		m_knockback *= KnockBackFallOffScalarPerFrame;

		if (m_knockback.SizeSquared() < KnockBackDisappearThreshold_Sqr) {
			m_GetDamageCoolDownFinish = true;
		}
	}

}
