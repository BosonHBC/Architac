// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Classes/GameFramework/SpringArmComponent.h"
#include "Classes/GameFramework/Actor.h"
#include "CameraControlComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ISLANDMAINPROJECT_API UCameraControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCameraControlComponent();

	UFUNCTION(BlueprintCallable)
		virtual void InitializeCameraComponent();

	// This is for testing
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool m_EnableTopdown = false;
	void ChangeCameraAngle();

protected:
	// Get owner's actor
	AActor* m_owner;
	// Variable for owner's camera
	UCameraComponent* m_ownersCamera;
	// Variable for owner's camera boom
	USpringArmComponent* m_ownersCameraBoom;

	// Temp value
	FRotator m_destRotator;

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		virtual void GetOwnersCameraComponents();

	UFUNCTION(BlueprintCallable)
		virtual void SetCamerasProperties();

	UFUNCTION(BlueprintCallable)
		void PerformCameraRotation(float deltaSeconds);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};