// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_MoveAlongCircleTrail.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Enemy/Trails/CircleTrail.h"

UBTTask_MoveAlongCircleTrail::UBTTask_MoveAlongCircleTrail()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveAlongCircleTrail::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

	aiCtrl->ResetChangeTrailTimer();

	return NodeResult;
}

void UBTTask_MoveAlongCircleTrail::OnGameplayTaskActivated(UGameplayTask& Task)
{

}

void UBTTask_MoveAlongCircleTrail::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

void UBTTask_MoveAlongCircleTrail::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.IsPaused()) {

		AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());
		// trail that this enemy in
		ACircleTrail* trail = aiCtrl->GetCircleTrail();

		aiCtrl->MoveAlongCircle(trail->GetCircleTangentAtLocation(aiCtrl->GetPawn()->GetActorLocation()), DeltaSeconds);

		EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
		NodeResult = PerformSwimmingTask(OwnerComp, NodeMemory, DeltaSeconds);
		if (NodeResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, NodeResult);
		}
	}
}

EBTNodeResult::Type UBTTask_MoveAlongCircleTrail::PerformSwimmingTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

	if (aiCtrl->IsTimeToChangeTrail()) {
		NodeResult = EBTNodeResult::Succeeded;
	}

	return NodeResult;
}
