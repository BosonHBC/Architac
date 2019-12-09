// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_PathToBuildingFound.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/Enemy/EnemyBase.h"

bool UBTDecorator_PathToBuildingFound::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == NULL)
	{
		return false;
	}

	AEnemyAIControllerBase* aiCtrl = Cast<AEnemyAIControllerBase>(OwnerComp.GetAIOwner());

	if (aiCtrl) {
		APawn* pawn = aiCtrl->GetPawn();
		if (pawn) {

			// When try moving to the target location, should not enable physics
			AEnemyBase* enemyBase = Cast<AEnemyBase>(pawn);
			enemyBase->ToggleSimulatePhysics(false);

			FHitResult outHit;
			FVector Start = pawn->GetActorLocation();
			FVector End = aiCtrl->GetCurrentTargetLocation();
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(pawn);

			if (GetWorld()->LineTraceSingleByChannel(outHit, Start, End, ECollisionChannel::ECC_Visibility, queryParams)) {
				// Hit the target
				if (outHit.Actor == aiCtrl->GetCurrentTarget()) {

				

					return true;
				}
			}
		}
	}
	return false;
}
