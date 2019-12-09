// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy/Trails/CircleTrail.h"
#include "DrawDebugHelpers.h"
// Sets default values
ACircleTrail::ACircleTrail()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FVector ACircleTrail::GetCircleTangentAtLocation(const FVector& i_location) const
{
	FVector toCenter = GetActorLocation() - i_location;
	return m_counterClockWise ? FVector::CrossProduct(toCenter, FVector::UpVector).GetSafeNormal() : FVector::CrossProduct(FVector::UpVector, toCenter).GetSafeNormal();
}

FVector ACircleTrail::GetClosestPointInCircle(const FVector& i_location) const
{
	FVector toLocation = i_location - GetActorLocation();
	toLocation.Z = 0;
	toLocation.Normalize();
	return toLocation * m_radius + GetActorLocation();
}

FVector ACircleTrail::GetRandomPointInCircle() const
{
	float theta = FMath::RandRange(0.f, 2.f * (float)PI);
	return FVector(m_radius * FMath::Sin(theta), m_radius * FMath::Cos(theta), 0) + GetActorLocation();
}

// Called when the game starts or when spawned
void ACircleTrail::BeginPlay()
{
	Super::BeginPlay();

	// Draw Debug Circle
	DrawDebugCircle(GetWorld(), GetActorLocation(), m_radius, 180, FColor::Red, true, -1, (uint8)'\000', 0.0, FVector(0, 1, 0), FVector(1, 0, 0), true);
}

// Called every frame
void ACircleTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

