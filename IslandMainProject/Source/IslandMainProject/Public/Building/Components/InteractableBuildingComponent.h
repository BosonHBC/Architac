// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "InteractableBuildingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndInteraction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterInteraction);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ISLANDMAINPROJECT_API UInteractableBuildingComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableBuildingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatch")
		FOnEnterInteraction m_OnEnterInteraction;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatch")
		FOnEndInteraction m_OnEndInteraction;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Interactable)
		void BPEnterInteraction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Interactable)
		void BPEndInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void EnterInteraction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void EndInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool m_isInteractble;
	UFUNCTION(BlueprintCallable, Category = "InteractblSystem")
		void BindCollisionComponent(UPrimitiveComponent* CollisionComp);
	UFUNCTION()
		void SetUpBuildingInfoToPlayer(bool isInteracting);
};

