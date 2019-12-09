// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Public/VenturePawn.h"
#include "Public/Constants/GameInputConstants.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "IslandMainProjectGameModeBase.h"
#include "Components/SphereComponent.h"

#include "Public/ToolMover.h"
#include "Public/PickUpBase.h"

#include "Public/InteractableBase.h"
#include "Public/InventoryComponent.h"
#include "Public/CameraControlComponent.h"

#define KnockBackFallOffScalarPerFrame 0.75f
#define KnockBackDisappearThreshold_Sqr 100
#define KnockBackScalar 100000

AVenturePawn::AVenturePawn()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Create a camera...
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComp->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Interact Point component
	InteractPointComp = CreateDefaultSubobject<USphereComponent>(TEXT("InteractPointComp"));
	InteractPointComp->SetupAttachment(RootComponent);
	InteractPointComp->SetSphereRadius(32.f);
	InteractPointComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	CameraControlComp = CreateDefaultSubobject<UCameraControlComponent>(TEXT("CameraControlComp"));

	// Damageable interface initialization
	{
		m_maxHealth = 200;
		m_currentHealth = 200;
		m_CanGetDamage = true;
		// Player should not damage by player it self
		m_DamageableMask.Add(EDamageSource::EDS_Player);
	}

	// Initialize the player to be a explore mode
	VenturePawnState = EVenturePlayerState::E_EXPLORE;
}

void AVenturePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateMoverTransformation();
	HandleKnockBack();
}

void AVenturePawn::BeginPlay()
{
	Super::BeginPlay();
	SpawnMover();
	OnMouseHold();
}

FIntVector AVenturePawn::GetKeyFromValueForRelationData(AActor* actor)
{
	FIntVector returnvector;
	for (auto& value : copyrelationdata)
	{
		if (value.Value == actor)
		{
			returnvector = value.Key;
		}
	}

	returnvector.X *= -1;
	returnvector.Y *= -1;
	returnvector.Z *= -1;

	return returnvector;
}


#pragma region PlayerInputFunctions
void AVenturePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AVenturePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVenturePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AVenturePawn::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AVenturePawn::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AVenturePawn::UnCrouch);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AVenturePawn::Interact);

	PlayerInputComponent->BindAction("Item1", IE_Pressed, this, &AVenturePawn::SwitchToItem1);
	PlayerInputComponent->BindAction("Item2", IE_Pressed, this, &AVenturePawn::SwitchToItem2);
	PlayerInputComponent->BindAction("Item3", IE_Pressed, this, &AVenturePawn::SwitchToItem3);
	PlayerInputComponent->BindAction("Item4", IE_Pressed, this, &AVenturePawn::SwitchToItem4);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AVenturePawn::OnMouseClick);
	PlayerInputComponent->BindAction("Action", IE_Released, this, &AVenturePawn::OnMouseRelease);
	PlayerInputComponent->BindAction("Communicate", IE_Pressed, this, &AVenturePawn::OnMouseRightClick);
	//PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AVenturePawn::DashToPlayerForward);

	//Initialize for CameraControl Component
	CameraControlComp->InitializeCameraComponent();
}

void AVenturePawn::MoveForward(float axis)
{
	if (VenturePawnState == EVenturePlayerState::E_EXPLORE)
	{
		FVector dir = CameraComp->GetForwardVector();
		dir.Z = 0;
		AddMovementInput(dir.GetSafeNormal() * axis);
	}
	else if (VenturePawnState == EVenturePlayerState::E_UIINTERACT)
	{
		if (InteractingBlockSet != nullptr)
		{
			if (axis > 0 && m_previousy <= 0)
				InteractingBlockSet->MoveIndicator(FIntVector(0, -1, 0), true);
			if (axis < 0 && m_previousy >= 0)
				InteractingBlockSet->MoveIndicator(FIntVector(0, +1, 0), true);
			m_previousy = axis;
		}
	}
}

void AVenturePawn::MoveRight(float axis)
{
	if (VenturePawnState == EVenturePlayerState::E_EXPLORE)
	{
		AddMovementInput(CameraComp->GetRightVector() * axis);
	}
	else if (VenturePawnState == EVenturePlayerState::E_UIINTERACT)
	{
		if (InteractingBlockSet != nullptr)
		{
			if (axis > 0 && m_previousx <= 0)
				InteractingBlockSet->MoveIndicator(FIntVector(1, 0, 0), true);
			if (axis < 0 && m_previousx >= 0)
				InteractingBlockSet->MoveIndicator(FIntVector(-1, 0, 0), true);
			m_previousx = axis;
		}
	}
}

void AVenturePawn::Jump()
{
	if (VenturePawnState == EVenturePlayerState::E_EXPLORE)
	{
		Super::Jump();
	}
	else if(VenturePawnState == EVenturePlayerState::E_UIINTERACT)
	{
	}
}

void AVenturePawn::Crouch()
{
	if (VenturePawnState == EVenturePlayerState::E_EXPLORE)
	{
		Super::Crouch();
	}
	else
	{

	}
}

void AVenturePawn::UnCrouch()
{
	if (VenturePawnState == EVenturePlayerState::E_EXPLORE)
	{
		Super::UnCrouch();
	}
	else
	{

	}
}



#pragma endregion

void AVenturePawn::OnMouseClick()
{
	m_toolMover->UseTool();
	m_OnMouseHold = true;

	if (VenturePawnState == EVenturePlayerState::E_UIINTERACT)
	{
		if (InteractingBlockSet != nullptr)
		{
			InteractingBlockSet->InteractWithUI();
		}
	}
}

void AVenturePawn::OnMouseHold()
{
	if (m_OnMouseHold) {
		m_toolMover->UseTool();
	}
}

void AVenturePawn::OnMouseRelease()
{
	m_OnMouseHold = false;
}

void AVenturePawn::OnMouseRightClick()
{

}

void AVenturePawn::Interact()
{
	UE_LOG(LogTemp, Log, TEXT("The current Player State is: %d"), VenturePawnState);
	if (InteractingBlockSet != nullptr)
	{
		if (InteractingBlockSet->CanProcessNextAction)
		{
			if (VenturePawnState == EVenturePlayerState::E_EXPLORE)
			{
				VenturePawnState = EVenturePlayerState::E_UIINTERACT;
				InteractingBlockSet->OpenWindow();
				CloseWindow();
			}
			else if (VenturePawnState == EVenturePlayerState::E_UIINTERACT)
			{
				VenturePawnState = EVenturePlayerState::E_EXPLORE;
				InteractingBlockSet->CloseWindow();
				OpenWindow();
			}
		}
	}
	else
	{
		VenturePawnState = EVenturePlayerState::E_EXPLORE;
	}
}

void AVenturePawn::SwitchToItem1()
{
	// Debug use
	m_toolMover->SwitchToTool(FName("Harpoon"));
	BPSwitchToItem1();
}

void AVenturePawn::SwitchToItem2()
{
	// Debug use
	m_toolMover->SwitchToTool(FName("Pickaxe"));
	BPSwitchToItem2();
}

void AVenturePawn::SwitchToItem3()
{
	// Debug use
	m_toolMover->SwitchToTool(FName("Hand"));
	BPSwitchToItem3();
}

void AVenturePawn::SwitchToItem4()
{
}

void AVenturePawn::SpawnMover()
{
	m_toolMover = GetWorld()->SpawnActor<AToolMover>(ToolMoverClass);
	m_toolMover->SetVenturePawnReference(this);
}

void AVenturePawn::UpdateMoverTransformation()
{
	if (m_toolMover) {
		m_toolMover->SetActorLocation(GetActorLocation() + MoverRelativeLocation);
		m_toolMover->SetActorRotation(GetActorRotation() + MoverRelativeRotatoin);
	}
}

void AVenturePawn::Die()
{
	//InventoryComp->ThrowAllItems();
}

void AVenturePawn::HandleKnockBack()
{
	// If it is getting damage
	if (!m_GetDamageCoolDownFinish) {
		// apply knock back vector
		AddMovementInput(m_knockback, 1.f);
		// knock back fall off
		m_knockback *= KnockBackFallOffScalarPerFrame;

		if (m_knockback.SizeSquared() < KnockBackDisappearThreshold_Sqr) {
			m_GetDamageCoolDownFinish = true;
		}
	}
}

void AVenturePawn::GetDamage_Implementation(const FDamageInfo& dInfo)
{
	// In the following case, it will not get damage
	// 1. last damage cool down is not finished yet
	// 2. It is not damageable
	// 3. the damage source is inside the damage mask
	if (!m_GetDamageCoolDownFinish || !m_CanGetDamage || m_DamageableMask.Contains(dInfo.DamageSource)) return;
	//Basic reduce health
	float actualDmg;
	if (dInfo.Damage > m_currentHealth)
		actualDmg = m_currentHealth;
	else actualDmg = dInfo.Damage;
	m_currentHealth -= actualDmg;
	UE_LOG(LogTemp, Log, TEXT("Player Getting attack!"));
	// Knock back
	m_knockback = dInfo.DamageDirection * KnockBackScalar;

	m_GetDamageCoolDownFinish = false;
	BPOnGetdamage(m_currentHealth / (float)m_maxHealth);
	if (m_currentHealth <= 0)
		Die();
}

void AVenturePawn::ChangeCharacterVelocity(float _speed)
{
	GetCharacterMovement()->MaxWalkSpeed = _speed;
}

void AVenturePawn::BlockCharacterRotation(float RotationRate_Yaw)
{
	GetCharacterMovement()->RotationRate.Yaw = RotationRate_Yaw;
}
