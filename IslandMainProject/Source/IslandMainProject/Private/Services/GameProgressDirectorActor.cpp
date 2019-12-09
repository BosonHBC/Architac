// Fill out your copyright notice in the Description page of Project Settings.


#include "GameProgressDirectorActor.h"
#include "UObject/UObjectIterator.h"

// Sets default values
AGameProgressDirectorActor::AGameProgressDirectorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

AMonsterSpawnServiceActor* AGameProgressDirectorActor::GetMonsterSpawner()
{
	return MonsterSpawnServiceActor;
}

void AGameProgressDirectorActor::SetMonsterSpawner(AMonsterSpawnServiceActor* pointer)
{
	MonsterSpawnServiceActor = pointer;
}

APickupSpawnServiceActor* AGameProgressDirectorActor::GetPickupSpawner()
{
	return PickupSpawnServiceActor;
}

void AGameProgressDirectorActor::SetPickupSpawner(APickupSpawnServiceActor* pointer)
{
	PickupSpawnServiceActor = pointer;
}

// Called when the game starts or when spawned
void AGameProgressDirectorActor::BeginPlay()
{
	Super::BeginPlay();

	//UWorld* world = GetWorld();
	//if (world)
	//{
	//	FActorSpawnParameters spawnParams;
	//	AMonsterSpawnServiceActor* monsterspawnactor = world->SpawnActor<AMonsterSpawnServiceActor>(AMonsterSpawnServiceActor::StaticClass(), GetActorLocation(), FRotator(0, 0, 0), spawnParams);
	//	MonsterSpawnServiceActor = monsterspawnactor;

	//	APickupSpawnServiceActor* pickupspawnactor = world->SpawnActor<APickupSpawnServiceActor>(APickupSpawnServiceActor::StaticClass(), GetActorLocation(), FRotator(0, 0, 0), spawnParams);
	//	PickupSpawnServiceActor = pickupspawnactor;
	//}
}

bool AGameProgressDirectorActor::SpawnTrackableMonster(TSubclassOf<AEnemyBase> monster, FVector location)
{
	if (IsValid(MonsterSpawnServiceActor))
	{
		AActor* trackableactor = MonsterSpawnServiceActor->SpawnMonster(monster, location);
		if (AEnemyBase * trackablemonster = Cast<AEnemyBase>(trackableactor))
		{
			return true;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a trackable monster for some reason"));
	return false;
}

bool AGameProgressDirectorActor::SpawnTrackableMonsterWithPickup(TSubclassOf<AEnemyBase> monster, FVector location, TSubclassOf<APickUpBase> pickup)
{
	if (IsValid(MonsterSpawnServiceActor))
	{
		AActor* trackableactor = MonsterSpawnServiceActor->SpawnMonsterWithPickup(monster, location, pickup);
		if (AEnemyBase* trackablemonster = Cast<AEnemyBase>(trackableactor))
		{
			trackablemonster->OnDropItems.AddDynamic(this, &AGameProgressDirectorActor::SpawnTrackablePickup);
			return true;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a trackable monster for some reason"));
	return false;
}

void AGameProgressDirectorActor::SpawnTrackablePickup(TSubclassOf<AActor> actor, FVector location)
{
	AActor* trackableobject = PickupSpawnServiceActor->SpawnItem(actor, location);
	if (APickUpBase* tracablemonster = Cast<APickUpBase>(trackableobject))
	{
		tracablemonster->OnPickedUp.AddDynamic(this, &AGameProgressDirectorActor::MarkAsAnUnique);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a trackable item for some reason"));
	}
}

void AGameProgressDirectorActor::MarkAsAnUnique(FName name)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("I spawned an unique item"));
}


void AGameProgressDirectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Called every frame
}