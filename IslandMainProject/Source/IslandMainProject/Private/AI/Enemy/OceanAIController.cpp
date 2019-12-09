// Fill out your copyright notice in the Description page of Project Settings.


#include "OceanAIController.h"
#include "AI/Enemy/OceanEnemy.h"
#define Sine_Amplitude 0.1f
#define Sine_Frequency 3.f


AOceanAIController::AOceanAIController()
{

}

void AOceanAIController::MoveAlongCircle(const FVector& i_direction, float DeltaTime)
{
	AEnemyBase* pEnemy = Cast<AEnemyBase>(GetPawn());

	float Z = Sine_Amplitude * FMath::Sin(Sine_Frequency * m_timeSinceStart);

	FVector sinDirection = FVector(i_direction.X, i_direction.Y, Z).GetSafeNormal();

	pEnemy->MoveTowardDirection(sinDirection, DeltaTime);
}

FVector AOceanAIController::GetCurrentTargetLocation()
{
	return m_currentTargetActor->GetActorLocation();
}
