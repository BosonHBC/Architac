// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_MoveToCircleTrail.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "AI/Enemy/EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Enemy/Trails/CircleTrail.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"



UBTTask_MoveToCircleTrail::UBTTask_MoveToCircleTrail()
{
	bNotifyTick = true;

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToCircleTrail, BlackboardKey));
}

EBTNodeResult::Type UBTTask_MoveToCircleTrail::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());
	
	// When try moving to the target location, should not enable physics
	AEnemyBase* pEnemy = Cast<AEnemyBase>(aiCtrl->GetPawn());
	pEnemy->ToggleSimulatePhysics(false);

	ACircleTrail* previourCircleTrail = aiCtrl->GetCircleTrail();
	APawn* pawn = aiCtrl->GetPawn();
	if (pawn) {
		ACircleTrail* fitTrail = nullptr;
		TArray<ACircleTrail*> _circleTrailInCertainLevel;
		// Find the suitable circle trail
		{
			for (TActorIterator<ACircleTrail> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// If this filtered level is the same as the Itr, store it in the list
				if (ActorItr->CircleTrailLevel == aiCtrl->GetFilteredHeightLevel())
					_circleTrailInCertainLevel.Add(*ActorItr);
			}
			// Can not find suitable trail with certain level
			if (_circleTrailInCertainLevel.Num() == 0) {
				NodeResult = EBTNodeResult::Failed;
				return NodeResult;
			}
			// Get a random non-repeat one
			int rndInt = FMath::RandRange(0, _circleTrailInCertainLevel.Num() - 1);
			int debugStop = 100;
			// when the previous one is null, or the previous one is the same as the incoming one
			while ((previourCircleTrail == nullptr || previourCircleTrail == _circleTrailInCertainLevel[rndInt]) && debugStop > 0)
			{
				rndInt = FMath::RandRange(0, _circleTrailInCertainLevel.Num() - 1);
				debugStop--;
			}
			fitTrail = _circleTrailInCertainLevel[rndInt];
		}

		if (fitTrail) {
			aiCtrl->SetUpMovingTrail(fitTrail);
		}
		else {
			// can't find circle trail
			NodeResult = EBTNodeResult::Failed;
		}
	}
	else {
		// can't find the controlled pawn
		NodeResult = EBTNodeResult::Failed;
	}
	return NodeResult;
}

void UBTTask_MoveToCircleTrail::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.IsPaused()) {

		AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

		aiCtrl->MoveDirectlyTo(DeltaSeconds);

		EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

		if (aiCtrl->IsCloesEnough()) {
			NodeResult = EBTNodeResult::Succeeded;
		}

		if (NodeResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, NodeResult);
		}
	}
}
