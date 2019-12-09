// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/AI/Enemy/Octopus/OctopusMinion.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Components\SkeletalMeshComponent.h"

#include "DrawDebugHelpers.h"
AOctopusMinion::AOctopusMinion()
{
	m_maxHealth = 250;
	m_currentHealth = m_maxHealth;

	AttackPoint = CreateDefaultSubobject<USphereComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(RootComponent);
	AttackPoint->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &AOctopusMinion::OnDamageableEnter);
	AttackPoint->SetSphereRadius(48.f);
	MoveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
	MoveComp->UpdatedComponent = RootComponent;

	MoveComp->MaxSpeed = 150.f;
	DamageDealt = 20;

	m_knockbackScalar = 5;
}

void AOctopusMinion::BeginPlay()
{
	Super::BeginPlay();
	// Disable at the beginning
	ToggleAttackPoint(false);
}

void AOctopusMinion::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AttackPoint->SetWorldLocation(SuperMesh->GetSocketLocation(FName("AttackPoint")));

}


// This function is for giving attack
void AOctopusMinion::OnDamageableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
			FDamageInfo dInfo;
			dInfo.Damage = DamageDealt;
			dInfo.DamageDirection = GetActorForwardVector();
			dInfo.DamageSource = EDamageSource::EDS_Enemy;
			dInfo.bDamaged = false;
			IDamageable::Execute_GetDamage(OtherActor, dInfo);
		}
	}
}


void AOctopusMinion::Die()
{
	Super::Die();
	ToggleAttackPoint(false);
}

void AOctopusMinion::PerformAttack()
{
	Super::PerformAttack();
	IsAttacking = true;
	// Enable Damage collision
	ToggleAttackPoint(true);
	// Show Animation
	BPPerformAttack();
}

void AOctopusMinion::ToggleAttackPoint(bool enable)
{
	// Set visual and collision
	//AttackPoint->bHiddenInGame = !enable;
	AttackPoint->SetCollisionEnabled( enable? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

}

void AOctopusMinion::FinishAttack()
{
	Super::PerformAttack();
	ToggleAttackPoint(false);
	IsAttacking = false;
}


