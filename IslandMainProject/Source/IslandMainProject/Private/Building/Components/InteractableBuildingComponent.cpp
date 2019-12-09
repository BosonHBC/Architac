// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBuildingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VenturePawn.h"
#include "Components/CapsuleComponent.h"
// Sets default values for this component's properties
UInteractableBuildingComponent::UInteractableBuildingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	/*auto SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	auto RootComponent = SceneComponent;*/
	PrimaryComponentTick.bCanEverTick = true;
	
	this->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	this->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	this->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	this->OnComponentBeginOverlap.AddDynamic(this, &UInteractableBuildingComponent::EnterInteraction);
	this->OnComponentEndOverlap.AddDynamic(this, &UInteractableBuildingComponent::EndInteraction);
	// ...
}


// Called when the game starts
void UInteractableBuildingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInteractableBuildingComponent::EndInteraction(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SetUpBuildingInfoToPlayer(false);

	//UE_LOG(LogTemp, Log, TEXT("end collinteraction"));
	m_OnEndInteraction.Broadcast();
	m_isInteractble = false;
	BPEndInteraction(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void UInteractableBuildingComponent::EnterInteraction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetUpBuildingInfoToPlayer(true);

	//UE_LOG(LogTemp, Log, TEXT("enter interaction"));
	m_OnEnterInteraction.Broadcast();
	m_isInteractble = true;
	BPEnterInteraction(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void UInteractableBuildingComponent::SetUpBuildingInfoToPlayer(bool isInteracting)
{
	// Register what building block a venture pawn is interact with
	AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (isInteracting)
	{
		if (AVenturePawn* venturepawn = Cast<AVenturePawn>(player))
		{
			AActor* owner = GetOwner();
			venturepawn->InteractingBlockSet = Cast<ABlockSet>(owner);
		}
	}
	else
	{
		if (AVenturePawn* venturepawn = Cast<AVenturePawn>(player))
		{
			AActor* owner = GetOwner();
			venturepawn->InteractingBlockSet = nullptr;
		}
	}
}


// Called every frame
void UInteractableBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UInteractableBuildingComponent::BindCollisionComponent(UPrimitiveComponent* CollisionComp)
{
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &UInteractableBuildingComponent::EnterInteraction);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &UInteractableBuildingComponent::EndInteraction);
}
