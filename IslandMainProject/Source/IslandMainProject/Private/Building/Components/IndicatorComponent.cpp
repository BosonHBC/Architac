// Fill out your copyright notice in the Description page of Project Settings.

#include "IndicatorComponent.h"
#include "IslandMainProjectGameModeBase.h"
#include "InventoryComponent.h"
#include "BlockSet.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UIndicatorComponent::UIndicatorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void UIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	PerceptionCapsuleComp = (USphereComponent*)GetOwner()->GetComponentByClass(USphereComponent::StaticClass());

	if (PerceptionCapsuleComp != nullptr)
	{
		PerceptionCapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PerceptionCapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		PerceptionCapsuleComp->SetSphereRadius(PerceptionRadius);

		PerceptionCapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &UIndicatorComponent::OnBeginPercept);
		PerceptionCapsuleComp->OnComponentEndOverlap.AddDynamic(this, &UIndicatorComponent::OnStopPercept);
	}
	// ...

	if (!UIIndicatorDataTable.IsNull())
	{
		m_UIindicatortablerow = *UIIndicatorDataTable.GetRow<FBuildingIndicationRow>("");
	}

	if (IndicatorState == EIndicatorState::E_INPUTTIP)
	{
		IsAlwayPerceptable = false;
		SetVisibility(false);
	}
	else if (IndicatorState == EIndicatorState::E_DAMAGE)
	{
		if (IsFrontSide)
		{
			rotation = FRotator(0, 180, 0);
		}

		AActor* actor = GetOwner();
		if (actor)
		{
			thisbuilding = Cast<ABlockSet>(actor);
		}
	}
	else if (IndicatorState == EIndicatorState::E_NOPWER)
	{
		if (IsFrontSide)
		{
			rotation = FRotator(0, 180, 0);
		}

		AActor* actor = GetOwner();
		if (actor)
		{
			thisbuilding = Cast<ABlockSet>(actor);
		}
	}
	else if (IndicatorState == EIndicatorState::E_RESOURCE)
	{
		if (IsFrontSide)
		{
			rotation = FRotator(0, 180, 0);
		}

		m_inventorycomponent = (UInventoryComponent*)GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass());
		m_generationcomponent = (UResourceGenerationComponent*)GetOwner()->GetComponentByClass(UResourceGenerationComponent::StaticClass());
	}
}


void UIndicatorComponent::OnBeginPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BPOnBeginPercept();

	if (IndicatorState == EIndicatorState::E_INPUTTIP)
	{
		SetVisibility(true);
	}
}

void UIndicatorComponent::OnStopPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	BPOnStopPercept();
	
	if (IndicatorState == EIndicatorState::E_INPUTTIP)
	{
		SetVisibility(false);
	}
}

// Called every frame
void UIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...

	if (IndicatorState == EIndicatorState::E_INPUTTIP)
	{

	}
	else
	{
		rotation = FRotator(0, rotation.Yaw + rotationspeed * DeltaTime, 0);
		this->SetWorldRotation(rotation);

		if (IndicatorState == EIndicatorState::E_DAMAGE)
		{
			if (thisbuilding != nullptr)
			{
				timer += DeltaTime;

				if (timer > 30)
				{	
					thisbuilding->m_currentHealth = thisbuilding->m_maxHealth;
				}

				if (thisbuilding->IsFullHealth())
				{
					texture = m_UIindicatortablerow.texturetype1;
					timer = 0.0f;
				}
				else
				{
					texture = m_UIindicatortablerow.texturetype2;
				}
			}
		}
		else if (IndicatorState == EIndicatorState::E_NOPWER)
		{
			if (thisbuilding != nullptr)
			{
 
				if (thisbuilding->GetBlockSetActiveStatus() ==  EBlockSetActiveStatus::ENABLED)
				{
					texture = m_UIindicatortablerow.texturetype1;

				}		
				else
				{
					texture = m_UIindicatortablerow.texturetype2;
				}
			}
		}
		else if (IndicatorState == EIndicatorState::E_RESOURCE)
		{
			if (m_generationcomponent != nullptr)
			{
				if (m_generationcomponent->factorystate == EFactoryState::E_ISFULL)
				{
					texture = m_UIindicatortablerow.texturetype1;
				}
				else if (m_generationcomponent->factorystate == EFactoryState::E_ISNOTFULL)
				{
					texture = m_UIindicatortablerow.texturetype2;
				}
			}
		}
	}
}

