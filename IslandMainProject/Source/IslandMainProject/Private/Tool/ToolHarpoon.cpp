// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Tool/ToolHarpoon.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

AToolHarpoon::AToolHarpoon()
{
	InteractPointComp->OnComponentBeginOverlap.AddDynamic(this, &AToolHarpoon::OnInteractableEnter);
	InteractPointComp->OnComponentEndOverlap.AddDynamic(this, &AToolHarpoon::OnInteractableLeft);

	Damage = 50.f;
}

void AToolHarpoon::UpdateDamage(float Dmg)
{
	Damage = Dmg;
}

void AToolHarpoon::BeginPlay()
{
	Super::BeginPlay();
	m_currentLocation = InteractPointComp->GetComponentLocation();
	m_previousLocation = m_currentLocation;
}

void AToolHarpoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GiveDamageToDamageable();
}

void AToolHarpoon::StartUseTool()
{
	Super::StartUseTool();
	m_canDealDmg = true;
	InteractPointComp->bHiddenInGame = false;
}

void AToolHarpoon::UseToolFinish()
{
	Super::UseToolFinish();
	m_canDealDmg = false;
	// Clean the map when it is finish using;
	m_damageableActorNbDamageDealtMap.Empty();
	InteractPointComp->bHiddenInGame = true;
}

void AToolHarpoon::GiveDamageToDamageable()
{
	if (m_canDealDmg) {
		// Update location info
		m_previousLocation = m_currentLocation;
		m_currentLocation = InteractPointComp->GetComponentLocation();

		for (auto It = m_damageableActorNbDamageDealtMap.CreateIterator(); It; ++It)
		{
			// If this actor is not getting damage yet, damage it
			// In this case, in this attack, any damageable actor entering this weapon will only get attack once.
			if (!It->Value.bDamaged) {
				IDamageable::Execute_GetDamage(It->Key, It->Value);
				It->Value.bDamaged = true;
			}
		}
	}
}

void AToolHarpoon::OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherComp != nullptr&& m_canDealDmg) {
		// This actor has Damageable event
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
			if (!m_damageableActorNbDamageDealtMap.Contains(OtherActor)) {
				// Add unique damageable to iteration list
				FDamageInfo dmgInfo;
				dmgInfo.Damage = Damage;
				dmgInfo.DamageSource = EDamageSource::EDS_Player;
				dmgInfo.DamageDirection = m_currentLocation - m_previousLocation;
				// Start to figure out where is the hit point
				TArray<FHitResult> AllResults;
				// Get the location of this actor
				auto Start = GetActorLocation();
				// Get the location of the other component
				auto End = OtherComp->GetComponentLocation();
				// Use a slightly larger radius to ensure we find the same result
				auto CollisionRadius = FVector::Dist(Start, End) * 1.1f;

				// Now do a spherical sweep to find the overlap
				GetWorld()->SweepMultiByObjectType(
					AllResults,
					Start,
					End,
					FQuat::Identity,
					0,
					FCollisionShape::MakeSphere(CollisionRadius),
					FCollisionQueryParams(false)
				);

				// Finally check which hit result is the one from this event
				for (auto HitResult : AllResults)
				{
					if (OtherComp->GetUniqueID() == HitResult.GetComponent()->GetUniqueID()) {
						// A component with the same UniqueID means we found our overlap!
						
						// Assign the impact point to the damage info
						dmgInfo.DamagePoint = HitResult.ImpactPoint;
						break;
					}
				}
				m_damageableActorNbDamageDealtMap.Add(OtherActor, dmgInfo);
			}
		}
	}
}

void AToolHarpoon::OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherComp != nullptr) {
	}
}
