// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveAlongCircleTrail.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UBTTask_MoveAlongCircleTrail : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

		UBTTask_MoveAlongCircleTrail();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	EBTNodeResult::Type PerformSwimmingTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaTime);
};
