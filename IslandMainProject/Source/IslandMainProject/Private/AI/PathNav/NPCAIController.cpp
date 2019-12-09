// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/AI/PathNav/NPCAIController.h"
#include "Public/AI/PathNav/NPCPathFollowingComponent.h"

ANPCAIController::ANPCAIController(const FObjectInitializer & ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UNPCPathFollowingComponent>(TEXT("PathFollowingComponent")))
{

}
