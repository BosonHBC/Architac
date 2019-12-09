// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasAttackRequest.generated.h"

/**
 *
 */
UCLASS()
class ISLANDMAINPROJECT_API UBTDecorator_HasAttackRequest : public UBTDecorator
{
	GENERATED_BODY()

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override{}
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override {}
};
