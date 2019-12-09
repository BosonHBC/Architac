// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_HasAttackRequest.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

bool UBTDecorator_HasAttackRequest::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == NULL)
	{
		return false;
	}

	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

	if (aiCtrl) {
		if (aiCtrl->m_GoingToAttackTarget && aiCtrl->GetCurrentTarget()) {
			// Set up value
			OwnerComp.GetBlackboardComponent()->SetValueAsObject("TargetActor", aiCtrl->GetCurrentTarget());
			return true;
		}
		else {
			// This attack request is invalid, clean it
			aiCtrl->CleanUpAttackRequest();
		}
	}
	return false;
}
