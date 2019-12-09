// Fill out your copyright notice in the Description page of Project Settings.


#include "OceanEnemy.h"
#include "Engine/World.h"
#include "EnemyLandPoint.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "IslandMainProjectGameModeBase.h"

#define RayCastBias 1.001
#define RayCastDistance 5500
#define LandTime 3

AOceanEnemy::AOceanEnemy()
{
	SwimSpeed = 300.f;
}


void AOceanEnemy::BeginPlay()
{
	Super::BeginPlay();
	SwimSpeed = FMath::RandRange(0, 200) + SwimSpeed;
}

void AOceanEnemy::MoveTowardDirection(const FVector& i_direction, float DeltaTime)
{
	Super::MoveTowardDirection(i_direction, DeltaTime);
	RootComponent->SetWorldRotation(FMath::Lerp(RootComponent->GetComponentRotation(), FRotationMatrix::MakeFromX(i_direction).Rotator(), 0.3f));
	SetActorLocation(i_direction*SwimSpeed*DeltaTime + GetActorLocation());
}

void AOceanEnemy::Die()
{
	AIslandMainProjectGameModeBase* GM = Cast<AIslandMainProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	GM->RemoveOceanEnemyRef(this);
	GM->SpawnNewOceanEnemy();
	Super::Die();

}
