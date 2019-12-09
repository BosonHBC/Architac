// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "InteractableBase.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API AInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

	virtual void Interact(FName toolid);
	virtual void UnInteract(FName toolid);
	void EnableInputHint();
	void DisableInputHint();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interactable)
		float PerceptionRadius = 50.0f;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Interactable)
		void BPInteract(FName toolid);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Interactable)
		void BPUnInteract(FName toolid);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Interactable)
		void BPStartPercept();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Interactable)
		void BPStopPercept();

	FORCEINLINE bool GetCanInteract()const { return m_canIntereact; }
	FORCEINLINE void  SetCanInteract(const bool canInteract) { m_canIntereact = canInteract; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool m_canIntereact;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* OverlapComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* PerceptionCapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* WidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MeshComp;

	UFUNCTION()
		void OnBeginPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnStopPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
