// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Libary/AILibary.h"
#include "CircleTrail.generated.h"



UCLASS()
class ISLANDMAINPROJECT_API ACircleTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACircleTrail();

	FVector GetCircleTangentAtLocation(const FVector& i_location) const;

	FVector GetClosestPointInCircle(const FVector& i_location) const;

	FVector GetRandomPointInCircle() const;

	/** The level where this trail in*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trail)
		ETrailHeightLevel CircleTrailLevel;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trail)
		float m_radius;
	/** If the creature move clockwise or counter-clockwise*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trail)
		bool m_counterClockWise;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
