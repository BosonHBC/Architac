// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControlComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Classes/GameFramework/Pawn.h"

// Sets default values for this component's properties
UCameraControlComponent::UCameraControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCameraControlComponent::InitializeCameraComponent()
{
	// Set the properties for camera components
	GetOwnersCameraComponents();
	SetCamerasProperties();

	if(m_owner->InputComponent != nullptr){

		APawn* pawn = Cast<APawn>(m_owner);
		m_owner->InputComponent->BindAxis("LookUp");
		m_owner->InputComponent->BindAxis("LookRight");
		m_owner->InputComponent->BindAxis("ZoomIn");
		m_owner->InputComponent->BindAction("ChangeCameraPerspective", IE_Pressed, this, &UCameraControlComponent::ChangeCameraAngle);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is an pawn doesn't get controller"));
	}
}

void UCameraControlComponent::ChangeCameraAngle()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
	m_EnableTopdown = true;
}

// Called when the game starts
void UCameraControlComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeCameraComponent();
}

void UCameraControlComponent::GetOwnersCameraComponents()
{
	m_owner = GetOwner();
	check(m_owner != nullptr);

	// Get m_owner's camera component
	TArray<UCameraComponent*> cameracomponents;
	m_owner->GetComponents(cameracomponents);
	check(cameracomponents.Num() == 1);
	if (cameracomponents.Num() == 1)
	{
		m_ownersCamera = cameracomponents[0];
	}

	// Get m_owner's spring arm component
	TArray<USpringArmComponent*> springarmcompnents;
	m_owner->GetComponents(springarmcompnents);
	check(springarmcompnents.Num() == 1);
	if (springarmcompnents.Num() == 1)
	{
		m_ownersCameraBoom = springarmcompnents[0];
	}

	return;
}

void UCameraControlComponent::SetCamerasProperties()
{

	// Create a camera boom...
	m_ownersCameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	m_ownersCameraBoom->TargetArmLength = 800.f;
	m_ownersCameraBoom->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	m_ownersCameraBoom->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level
	m_ownersCameraBoom->bUsePawnControlRotation = false;
	m_ownersCameraBoom->bInheritPitch = false;
	m_ownersCameraBoom->bInheritRoll = false;
	m_ownersCameraBoom->bInheritYaw = false;

		// Create a camera...
	m_ownersCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void UCameraControlComponent::PerformCameraRotation(float deltaSeconds)
{
	// Rotate screen along with a mouse movement
	if (m_owner->InputComponent) 
	{
		if (!m_EnableTopdown)
			m_ownersCameraBoom->AddRelativeRotation(FRotator((m_owner->InputComponent->GetAxisValue("LookUp")), m_owner->InputComponent->GetAxisValue("LookRight"), 0.f));
		else
			m_ownersCameraBoom->AddRelativeRotation(FRotator(0.0f, m_owner->InputComponent->GetAxisValue("LookRight"), 0.0f));

		// Zoom in to a screen based on mouse wheel axis, -10 is a magic number to adjust intensity
		m_ownersCameraBoom->TargetArmLength += -5 * m_owner->InputComponent->GetAxisValue("ZoomIn");
	}

	FRotator fixedrotator;
	fixedrotator.Roll = 0.0f;
	fixedrotator.Pitch = m_ownersCameraBoom->GetTargetRotation().Pitch;
	fixedrotator.Yaw = m_ownersCameraBoom->GetTargetRotation().Yaw;

	if (fixedrotator.Pitch < -85)
		fixedrotator.Pitch = -85;
	if (fixedrotator.Pitch > 85)
		fixedrotator.Pitch = 85;

	m_ownersCameraBoom->SetWorldRotation(fixedrotator);
}


// Called every frame
void UCameraControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
	PerformCameraRotation(DeltaTime);
}
