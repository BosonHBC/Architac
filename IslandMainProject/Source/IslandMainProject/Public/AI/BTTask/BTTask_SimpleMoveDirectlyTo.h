// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SimpleMoveDirectlyTo.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UBTTask_SimpleMoveDirectlyTo : public UBTTaskNode
{
	GENERATED_BODY()
	
		UBTTask_SimpleMoveDirectlyTo();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override {}

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
