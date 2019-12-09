// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy/FatBird/FatBird.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SkyAIController.h"


AFatBird::AFatBird()
{
	m_maxHealth = 150;
	m_currentHealth = m_maxHealth;

	AttackPoint = CreateDefaultSubobject<USphereComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(RootComponent);
	AttackPoint->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &AFatBird::OnDamageableEnter);
	AttackPoint->SetSphereRadius(48.f);
	MoveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
	MoveComp->UpdatedComponent = RootComponent;
	

	FlyingSpeed = 250.f;
	MoveComp->MaxSpeed = 250.f;
	DamageDealt = 100;

}

void AFatBird::BeginPlay()
{
	Super::BeginPlay();
	ToggleAttackPoint(false);
}

void AFatBird::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AttackPoint->SetWorldLocation(SuperMesh->GetSocketLocation(FName("AttackPoint")));

	m_previousLocation = m_currentLocation;
	FVector2D newLoc = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	m_currentLocation = newLoc;
	m_horizontalVelocitySqr = (m_currentLocation - m_previousLocation).SizeSquared();
}

bool AFatBird::IsTargetOnBottom()
{
	ASkyAIController* skyEnemy = Cast<ASkyAIController>(GetController());
	if (skyEnemy) {
		AActor* targetActor = skyEnemy->GetCurrentTarget();
		if (targetActor) {
			FVector dirToTarget = targetActor->GetActorLocation() - GetActorLocation();
			return FVector::DotProduct(dirToTarget, FVector::UpVector) < 0;
		}
	}
	return false;
}

void AFatBird::OnDamageableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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


void AFatBird::ToggleAttackPoint(bool enable)
{
	//AttackPoint->bHiddenInGame = !enable;
	AttackPoint->SetCollisionEnabled(enable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

}

void AFatBird::FinishAttack()
{
	Super::FinishAttack();
	ToggleAttackPoint(false);
	IsAttacking = false;

}

void AFatBird::PerformAttack()
{
	Super::PerformAttack();
	IsAttacking = true;
	// Enable Damage collision
	ToggleAttackPoint(true);
	// Show Animation
	BPPerformAttack();
}
