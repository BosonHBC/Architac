// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyAIController.h"
#include "AI/Enemy/SkyEnemy.h"
#include "Building/Actors/BlockSet.h"
#include "Building/Components/CreatureLandPoint.h"

void ASkyAIController::MoveAlongCircle(const FVector& i_direction, float DeltaTime)
{
	AEnemyBase* pEnemy = Cast<AEnemyBase>(GetPawn());

	float Z = 0.5f * FMath::Sin(4.f * m_timeSinceStart);

	FVector sinDirection = FVector(i_direction.X, i_direction.Y, Z).GetSafeNormal();

	pEnemy->MoveTowardDirection(sinDirection, DeltaTime);
}

FVector ASkyAIController::GetCurrentTargetLocation()
{
	ABlockSet* target = Cast<ABlockSet>(m_currentTargetActor);
	if (target && target->GetLandPoint() != nullptr)
		return target->GetLandPoint()->GetComponentLocation();
	return FVector::ZeroVector;
}

bool ASkyAIController::IsLanded()
{
	ASkyEnemy* pEnemy = Cast<ASkyEnemy>(GetPawn());
	m_isLanded = pEnemy->CheckIsLanded();
	return m_isLanded;
}

void ASkyAIController::StartLand()
{
	ASkyEnemy* pEnemy = Cast<ASkyEnemy>(GetPawn());
	pEnemy->ToggleSimulatePhysics(true);
}
