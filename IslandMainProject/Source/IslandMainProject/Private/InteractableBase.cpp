// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/InteractableBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Public/VenturePawn.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	RootComponent = MeshComp;

	OverlapComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetupAttachment(RootComponent);

	PerceptionCapsuleComp = CreateDefaultSubobject<USphereComponent>(TEXT("PerceptionCapsulePerceptionComp"));
	PerceptionCapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PerceptionCapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	PerceptionCapsuleComp->SetSphereRadius(PerceptionRadius);
	PerceptionCapsuleComp->SetupAttachment(RootComponent);
	
	PerceptionCapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AInteractableBase::OnBeginPercept);
	PerceptionCapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AInteractableBase::OnStopPercept);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetVisibility(false);
	WidgetComp->SetupAttachment(RootComponent);
}

void AInteractableBase::Interact(FName toolid)
{
	if (!m_canIntereact) return;
}

void AInteractableBase::UnInteract(FName toolid)
{
	if (!m_canIntereact) return;
}

void AInteractableBase::EnableInputHint()
{
	// Set the input in the interact widget
	WidgetComp->SetVisibility(true);
}

void AInteractableBase::DisableInputHint()
{
	WidgetComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractableBase::OnBeginPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		AVenturePawn * player = Cast<AVenturePawn>(OtherActor);
		if (player)
		{
			EnableInputHint();
			BPStartPercept();
		}
	}
}

void AInteractableBase::OnStopPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		AVenturePawn* player = Cast<AVenturePawn>(OtherActor);
		if (player)
		{
			DisableInputHint();
			BPStopPercept();
		}
	}
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

