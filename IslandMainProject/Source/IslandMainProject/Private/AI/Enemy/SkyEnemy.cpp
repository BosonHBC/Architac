// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy/SkyEnemy.h"
#include "DrawDebugHelpers.h"
#include "IslandMainProjectGameModeBase.h"
#define LineTraceLength 200
#define OnGroundThreshold 5

ASkyEnemy::ASkyEnemy()
{
	
	m_collisionParam.AddIgnoredComponent(CapComp);
}

void ASkyEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ASkyEnemy::MoveTowardDirection(const FVector& i_direction, float DeltaTime)
{
	Super::MoveTowardDirection(i_direction, DeltaTime);
	AddMovementInput(i_direction, FlyingSpeed);
	RootComponent->SetWorldRotation(FMath::Lerp(RootComponent->GetComponentRotation(), FRotationMatrix::MakeFromX(i_direction).Rotator(), 0.3f));
	//SetActorLocation(i_direction*FlyingSpeed*DeltaTime + GetActorLocation());
}

bool ASkyEnemy::CheckIsLanded()
{
	bool result = false;
	FHitResult outHit;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - GetActorUpVector() * LineTraceLength;
	FColor hitColor = FColor::Red;

	if (GetWorld()->LineTraceSingleByChannel(outHit, Start, End, ECollisionChannel::ECC_Visibility, m_collisionParam)) {
		// Can land on ground

		float distSqrToGroundMesh = FVector::DistSquared(Start, outHit.ImpactPoint);
		float halfHeight = CapComp->GetScaledCapsuleHalfHeight() + OnGroundThreshold;
		if (distSqrToGroundMesh < halfHeight*halfHeight) {
			result = true;
			End = outHit.ImpactPoint;
			hitColor = FColor::Green;
		}
	}

	DrawDebugLine(GetWorld(), Start, End, hitColor);
	return result;
}

void ASkyEnemy::Die()
{

	AIslandMainProjectGameModeBase* GM = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	GM->RemoveSkyEnemyRef(this);
	GM->SpawnNewSkyEnemy();
	Super::Die();

}

