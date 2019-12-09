// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToCircleTrail.generated.h"

/**
 *
 */
UCLASS()
class ISLANDMAINPROJECT_API UBTTask_MoveToCircleTrail : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
		UBTTask_MoveToCircleTrail();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override {}

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
