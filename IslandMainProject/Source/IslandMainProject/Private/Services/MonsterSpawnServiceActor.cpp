// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSpawnServiceActor.h"
#include "AI/Enemy/EnemyBase.h"

AMonsterSpawnServiceActor::AMonsterSpawnServiceActor()
{

}

AActor* AMonsterSpawnServiceActor::SpawnMonster(TSubclassOf<AActor> monster, FVector location)
{
	if (monster)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			FRotator rotator;
			//spawnParams.bDeferConstruction = true;
			AActor* object = world->SpawnActor<AActor>(monster, location, rotator, spawnParams);
			return object;
		}
		return nullptr;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Something goes wrong with spawning monster"));
		return nullptr;
	}
}

AActor* AMonsterSpawnServiceActor::SpawnMonsterWithPickup(TSubclassOf<AActor> monster, FVector location, TSubclassOf<APickUpBase> pickup)
{
	if (monster)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			FRotator rotator;
			spawnParams.bDeferConstruction = true;
			AActor* object = world->SpawnActor<AActor>(monster, location, rotator, spawnParams);

			if (AEnemyBase* enemy = Cast<AEnemyBase>(object))
			{
				enemy->InjectUniqueItem(pickup);
			}

			FTransform objecttransformation;
			objecttransformation.SetLocation(location);
			object->FinishSpawning(objecttransformation);
			return object;
		}
		return nullptr;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Something goes wrong with spawning monster"));
		return nullptr;
	}
}

void AMonsterSpawnServiceActor::SpawnMonsers(TArray<TSubclassOf<AActor>> monsters, FVector location)
{
	for (int i = 0; i < monsters.Num() ; i++)
	{
		if (m_storedObjects.Num() <= MaxExistAmount)
		{
			AActor* actor = SpawnMonster(monsters[i], GetRandomLocationAroundPoint(location, 5));
			if (AEnemyBase* monster = Cast<AEnemyBase>(actor))
			{
				m_storedObjects.Add(actor);
				monster->OnGetKilled.AddDynamic(this, &AObjectSpawnServiceActor::CleanUpStoredObjects);
			}
		}
		else
		{

		}
	}
}

void AMonsterSpawnServiceActor::MassSpawning(TArray<TSubclassOf<AActor>> monsters, FVector location)
{
	m_timerDel.BindUFunction(this, FName("SpawnMonsers"), monsters, location);
	GetWorldTimerManager().SetTimer(m_spawnTimeHandle, m_timerDel, SpawnPeriod, true, SpawnPeriod);
}

FVector AMonsterSpawnServiceActor::GetRandomLocationAroundPoint(FVector location, float radius)
{
	return location;
}
