// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StaticFunctionLibary.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API UStaticFunctionLibary : public UObject
{
	GENERATED_BODY()
public:



		//************************************
		// Method:    KnockBackActor
		// FullName:  UStaticFunctionLibary::KnockBackActor
		// Access:    public static 
		// Returns:   void
		// Qualifier:
		// Parameter: AActor * Player, actor that will get knocked back
		// Parameter: FVector IncomeForceDirection, the direction that the actor will be knocked to
		// Parameter: float force, an indication of the knock back distance 
		// Parameter: float knockBackDuration, the time will spend on performing this knock back
		// Parameter: FName KnockBackCallback, the call back function is call when knock back is done
		//************************************
		UFUNCTION(BlueprintCallable, Category = "CombatSystem")
		static void KnockBackActor(AActor* Player, FVector IncomeForceDirection, float force, float knockBackDuration, FName KnockBackCallback, int32 _UUID);


		static bool IsValid(UObject* Obj);

};
