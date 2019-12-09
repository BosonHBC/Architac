// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_ViewBlocked.h"
#include "AI/Enemy/EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"


bool UBTDecorator_ViewBlocked::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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
			FHitResult outHit;
			FVector Start = pawn->GetActorLocation();
			FVector End = pawn->GetActorLocation() + pawn	->GetActorForwardVector() * 300.f;
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(pawn);

			if (GetWorld()->LineTraceSingleByChannel(outHit, Start, End, ECollisionChannel::ECC_Visibility, queryParams)) {
				// Hit any obstacle
				return true;
			}
		}
	}
	return false;
}
