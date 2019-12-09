// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_AttackPlayer.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "AI/Enemy/EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#define AttackRange_Sqr 40000

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

	AEnemyBase* enemy = Cast<AEnemyBase>(aiCtrl->GetPawn());
	if (!enemy) {
		// It is not attackable enemy
		NodeResult = EBTNodeResult::Failed;
		return NodeResult;
	}

	float distSqr = FVector::DistSquared(aiCtrl->GetCurrentTargetLocation() , enemy->GetActorLocation());
	if (distSqr < AttackRange_Sqr) {
		enemy->PerformAttack();
	}
	else {
		// Need to get closer
		NodeResult = EBTNodeResult::Failed;
	}
	return NodeResult;
}

void UBTTask_AttackPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.IsPaused()) {
		EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
		AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());
		AEnemyBase* enemy = Cast<AEnemyBase>(aiCtrl->GetPawn());

		// When it is not attacking
		if (!enemy->IsAttacking) {
			NodeResult = EBTNodeResult::Succeeded;
		}

		if (NodeResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, NodeResult);
		}

	}
}
