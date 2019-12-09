// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_SimpleMoveDirectlyTo.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Public/AI/Enemy/EnemyBase.h"

#define StopThreshold_Sqr 10000

UBTTask_SimpleMoveDirectlyTo::UBTTask_SimpleMoveDirectlyTo()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SimpleMoveDirectlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());
	
	// When try moving to the target location, should not enable physics
	AEnemyBase* pEnemy = Cast<AEnemyBase>(aiCtrl->GetPawn());
	pEnemy->ToggleSimulatePhysics(false);

	return EBTNodeResult::InProgress;
}

void UBTTask_SimpleMoveDirectlyTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.IsPaused()) {

		AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

		AEnemyBase* pEnemy = Cast<AEnemyBase>(aiCtrl->GetPawn());
		FVector tarLoc = aiCtrl->GetCurrentTargetLocation();
		pEnemy->MoveTowardDirection((tarLoc - pEnemy->GetActorLocation()).GetSafeNormal(), DeltaSeconds);

		EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

		if (FVector::DistSquared(tarLoc, pEnemy->GetActorLocation()) < StopThreshold_Sqr) {
			NodeResult = EBTNodeResult::Succeeded;
		}

		if (!aiCtrl->m_GoingToAttackTarget || !aiCtrl->GetCurrentTarget()) {
			NodeResult = EBTNodeResult::Failed;
		}

		if (NodeResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, NodeResult);
		}
	}
}
