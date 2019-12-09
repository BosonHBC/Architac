// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectSpawnServiceActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "PickUpBase.h"


// Sets default values
AObjectSpawnServiceActor::AObjectSpawnServiceActor()
{
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetSimulatePhysics(false);
	SpawnArea->SetupAttachment(RootComponent);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

//void AObjectSpawnServiceActor::PopObject()
//{
//	TSubclassOf<class AActor> pop;
//	ObjectQueue.Dequeue(pop);
//	SpawnObject(pop);
//}

//AActor * AObjectSpawnServiceActor::SpawnObject(TSubclassOf<class AActor> spawnobject)
//{
//	if (spawnobject)
//	{
//		UWorld* world = GetWorld();
//		if (world)
//		{
//			FActorSpawnParameters spawnParams;
//			FRotator rotator;
//
//			AActor* object = world->SpawnActor<AActor>(spawnobject, GetRandomLocation(), rotator, spawnParams);
//			m_storedObjects.Add(object);
//			return object;
//		}
//		return nullptr;
//	}
//	else
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("There is no item to spawn from spawning service actor"));
//		return nullptr;
//	}
//}

FVector AObjectSpawnServiceActor::GetRandomLocation()
{
	FVector area = SpawnArea->GetScaledBoxExtent();
	float spawnx = FMath::RandRange(-1 * area.X, area.X);
	float spawny = FMath::RandRange(-1 * area.Y, area.Y);

	FVector spawnpoint = GetActorLocation() + FVector(spawnx, spawny, 0);

	return spawnpoint;
}

//bool AObjectSpawnServiceActor::EnqueueObject(int objectnumber)
//{
//	if (ObjectQueue.Enqueue(ObjectsBlueprints[objectnumber]))
//	{
//		return true;
//	}
//	return false;
//}

void AObjectSpawnServiceActor::ChangeTimerTime(float time)
{
	GetWorld()->GetTimerManager().ClearTimer(m_spawnTimeHandle);
	m_timerDel.BindUFunction(this, FName("MassSpawnObject"), SpawnAmountAtOneTime, 2);
	GetWorldTimerManager().SetTimer(m_spawnTimeHandle, m_timerDel, time, true);
}

//void AObjectSpawnServiceActor::MassSpawnObject(int numberofamount, float spawnlag)
//{
//	CleanUpStoredObjects();
//	if (m_storedObjects.Num() >= MaxExistAmount)
//	{
//		ChangeTimerTime(3);
//		return;
//	}
//	if (spawnlag < 0)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Worng spawnlag parameter"));
//		return;
//	}
//	for (int i = 0; i < numberofamount; i++)
//	{
//		float randomtime = FMath::FRandRange(0, spawnlag);
//		int randomitem = FMath::RandRange(0, ObjectsBlueprints.Num() -1 );
//		SpawnObject(ObjectsBlueprints[randomitem]);
//	}
//	
//	ChangeTimerTime(SpawnPeriod);
//}

// Called when the game starts or when spawned
void AObjectSpawnServiceActor::BeginPlay()
{
	Super::BeginPlay();
}

void AObjectSpawnServiceActor::CleanUpStoredObjects()
{
	for (int i = 0; i < m_storedObjects.Num(); i++)
	{
		if (m_storedObjects[i] != nullptr)
		{
			if (m_storedObjects[i]->IsValidLowLevel())
			{
				if (!m_storedObjects[i]->IsPendingKill())
				{
					m_storedObjects.RemoveAt(i);
				}
				else
				{
					m_storedObjects.RemoveAt(i);
				}
			}
			else
			{
				m_storedObjects.RemoveAt(i);
			}
		}
		else
		{
			m_storedObjects.RemoveAt(i);
		}
	}
}

// Called every frame
void AObjectSpawnServiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

