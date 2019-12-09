// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/Enemy/EnemyBase.h"
#include "DrawDebugHelpers.h"
#include "AI/Enemy/Trails/CircleTrail.h"
#define Arrive_Threshold 15.f
#define Buffer_Distance 1500.f

AEnemyAIControllerBase::AEnemyAIControllerBase()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard Comp"));
	m_durationToChangeTrail = 5.f + FMath::RandRange(0.f, 5.f);
}

void AEnemyAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemyBase* pEnemy = Cast<AEnemyBase>(InPawn);
	UE_LOG(LogTemp, Log, TEXT("On posses AI: %s"), *pEnemy->GetName());
	if (pEnemy && pEnemy->GetBehaviourTree()) {
		Blackboard->InitializeBlackboard(*(pEnemy->GetBehaviourTree()->BlackboardAsset));
		if (!RunBehaviorTree(pEnemy->GetBehaviourTree())) {
			UE_LOG(LogTemp, Log, TEXT("Run behavior tree fail!"));
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("Run behavior tree Succeed!%s"), *pEnemy->GetBehaviourTree()->GetName());
		}
	}
	

}

void AEnemyAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_timeSinceStart += DeltaTime;

	// if it is not the time to change trail, add up the time.
	if (!IsTimeToChangeTrail()) m_collapseTime += DeltaTime;
}

void AEnemyAIControllerBase::StopRunAILogic(const FString i_reason)
{
	BrainComponent->StopLogic(i_reason);
}

void AEnemyAIControllerBase::SetAttackTarget(AActor* i_target)
{
	m_GoingToAttackTarget = true;
	m_currentTargetActor = i_target;
}

void AEnemyAIControllerBase::CleanUpAttackRequest()
{
	m_GoingToAttackTarget = false;
	m_currentTargetActor = nullptr;
}

void AEnemyAIControllerBase::MoveAlongCircle(const FVector& i_direction, float DeltaTime)
{
	// Parent implementation
	// ...
}

void AEnemyAIControllerBase::MoveDirectlyTo(float DeltaTime)
{
	AEnemyBase* pEnemy = Cast<AEnemyBase>(GetPawn());
	if (!pEnemy) return;

	FVector dir = (m_cloestPoint - pEnemy->GetActorLocation()).GetSafeNormal();
	pEnemy->MoveTowardDirection(dir, DeltaTime);
}

void AEnemyAIControllerBase::SetUpMovingTrail(ACircleTrail* i_trail)
{
	m_FollowingTrail = i_trail;
	APawn* pawn = GetPawn();
	FVector bufferPoint = pawn->GetActorLocation() + pawn->GetActorForwardVector() * Buffer_Distance;
	m_cloestPoint = i_trail->GetClosestPointInCircle(bufferPoint);
	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), m_cloestPoint, FColor::Green, true, m_durationToChangeTrail);
}

bool AEnemyAIControllerBase::IsTimeToChangeTrail() const
{
	return m_collapseTime > m_durationToChangeTrail;
}

void AEnemyAIControllerBase::ResetChangeTrailTimer()
{
	m_collapseTime = 0;
	m_durationToChangeTrail = 5.f + FMath::RandRange(0.f, 5.f);

}

bool AEnemyAIControllerBase::IsCloesEnough() const
{
	return FVector::Dist(GetPawn()->GetActorLocation(), m_cloestPoint) <= Arrive_Threshold;

}
