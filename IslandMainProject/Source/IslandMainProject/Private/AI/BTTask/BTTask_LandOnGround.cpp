// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_LandOnGround.h"
#include "SkyAIController.h"
UBTTask_LandOnGround::UBTTask_LandOnGround()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_LandOnGround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	ASkyAIController* aiCtrl = Cast<ASkyAIController>(OwnerComp.GetAIOwner());
	// Enable simulate physic
	aiCtrl->StartLand();
	return NodeResult;
}

void UBTTask_LandOnGround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.IsPaused()) {

		ASkyAIController* aiCtrl = Cast<ASkyAIController>(OwnerComp.GetAIOwner());
		EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
		if (aiCtrl->IsLanded()) {
			NodeResult = EBTNodeResult::Succeeded;
		}

		if (NodeResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, NodeResult);
		}
	}
}
