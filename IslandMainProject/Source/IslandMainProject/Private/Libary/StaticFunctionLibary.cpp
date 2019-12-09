// Fill out your copyright notice in the Description page of Project Settings.


#include "Libary/StaticFunctionLibary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"



void UStaticFunctionLibary::KnockBackActor(AActor* Player, FVector IncomeForceDirection, float force, float knockBackDuration = 0.2f, FName KnockBackCallback = FName("None"), int32 _UUID = 123)
{
	FVector offset = IncomeForceDirection * force;
	FVector DestLocation = Player->GetRootComponent()->RelativeLocation + offset;
	TEnumAsByte<EMoveComponentAction::Type> MoveAction;
	MoveAction = EMoveComponentAction::Move;
	// Set up latent action
	FLatentActionInfo LatentInfo;
	if (KnockBackCallback != FName("None")) {
		LatentInfo.CallbackTarget = Player;
		LatentInfo.ExecutionFunction = KnockBackCallback;
		LatentInfo.UUID = _UUID;
		LatentInfo.Linkage = 0;
	}


	UKismetSystemLibrary::MoveComponentTo(Player->GetRootComponent(), DestLocation, Player->GetActorRotation(), true, false, knockBackDuration, false, MoveAction, LatentInfo);
}

bool UStaticFunctionLibary::IsValid(UObject* Obj)
{
	if (!Obj)
	{
		return false;
	}
	if (!Obj->IsValidLowLevel())
	{
		return false;
	}
	if (Obj->IsPendingKill())
	{
		return false;
	}
	return true;

}
