// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackBuilding.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UBTTask_AttackBuilding : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_AttackBuilding();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override {}
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
