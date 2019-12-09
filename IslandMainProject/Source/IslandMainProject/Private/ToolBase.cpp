// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/ToolBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Public/ToolMover.h"

// Sets default values
AToolBase::AToolBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SuperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SuperMesh"));
	SuperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SuperMesh;

	InteractPointComp = CreateDefaultSubobject<USphereComponent>(TEXT("InteractPoint"));
	InteractPointComp->OnComponentBeginOverlap.AddDynamic(this, &AToolBase::OnInteractableEnterBP);
	InteractPointComp->OnComponentEndOverlap.AddDynamic(this, &AToolBase::OnInteractableLeftBP);
	InteractPointComp->SetupAttachment(SuperMesh);

	InteractPointComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void AToolBase::StartUseTool()
{
	if (m_using || !m_enable) return;
	m_using = true;
	DuringInteractable();
}

void AToolBase::UseToolFinish()
{
//	if (!m_using) return;
	m_using = false;
}

void AToolBase::SetMover(class AToolMover* mover)
{
	m_parentMover = mover;
}

void AToolBase::ToggleTool(bool enable)
{
	m_enable = enable;
	SuperMesh->SetVisibility(enable);
	InteractPointComp->SetCollisionEnabled(enable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AToolBase::BeginPlay()
{
	Super::BeginPlay();

}

void AToolBase::OnInteractableEnter(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void AToolBase::OnInteractableLeft(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AToolBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
