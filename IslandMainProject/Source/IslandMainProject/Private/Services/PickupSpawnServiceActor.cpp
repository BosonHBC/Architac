// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawnServiceActor.h"

APickupSpawnServiceActor::APickupSpawnServiceActor()
{

}

AActor* APickupSpawnServiceActor::SpawnItem(TSubclassOf<class AActor> spawnitem, FVector itemlocation)
{
	if (spawnitem)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			FRotator rotator;
			AActor* object = world->SpawnActor<AActor>(spawnitem, itemlocation, rotator, spawnParams);
			return object;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("There is no item to spawn from item spawn service actor"));
	}
	return nullptr;
}
