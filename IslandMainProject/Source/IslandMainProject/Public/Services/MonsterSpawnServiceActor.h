// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectSpawnServiceActor.h"
#include "PickUpBase.h"
#include "MonsterSpawnServiceActor.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API AMonsterSpawnServiceActor : public AObjectSpawnServiceActor
{
	GENERATED_BODY()

public:
	AMonsterSpawnServiceActor();

	UFUNCTION()
		AActor* SpawnMonster(TSubclassOf<AActor> monster, FVector location);

	UFUNCTION()
		AActor* SpawnMonsterWithPickup(TSubclassOf<AActor> monster, FVector location, TSubclassOf<APickUpBase> pickup);

	UFUNCTION()
		void SpawnMonsers(TArray<TSubclassOf<AActor>> monsters, FVector location);

	// This mass spawning will track the pack of monsters
	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		void MassSpawning(TArray<TSubclassOf<AActor>> monsters, FVector location);

protected:
	
	UFUNCTION()
		FVector GetRandomLocationAroundPoint(FVector location, float radius);
};
