// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyLandPoint.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API AEnemyLandPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyLandPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Point")
		bool m_airLandPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
