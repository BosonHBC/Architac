// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectSpawnServiceActor.h"
#include "PickupSpawnServiceActor.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAINPROJECT_API APickupSpawnServiceActor : public AObjectSpawnServiceActor
{
	GENERATED_BODY()

public:
	APickupSpawnServiceActor();

	UFUNCTION(BlueprintCallable, Category = "SpawnService")
		AActor* SpawnItem(TSubclassOf<class AActor> spawnitem, FVector itemlocation);
	
};
