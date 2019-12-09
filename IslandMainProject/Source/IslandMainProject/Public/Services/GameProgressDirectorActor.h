// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectSpawnServiceActor.h"
#include "PickUpBase.h"
#include "Services/MonsterSpawnServiceActor.h"
#include "Services/PickupSpawnServiceActor.h"
#include "Public/AI/Enemy/EnemyBase.h"
#include "GameProgressDirectorActor.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API AGameProgressDirectorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameProgressDirectorActor();

	//FORCEINLINE class AMonsterSpawnServiceActor* GetMonsterSpawner() const
	//{return MonsterSpawnServiceActor;}

	//FORCEINLINE class APickupSpawnServiceActor* GetPickupSpawner() const
	//{return PickupSpawnServiceActor;}

	UFUNCTION(BlueprintCallable, Category = GameProgressManagement)
		AMonsterSpawnServiceActor* GetMonsterSpawner();

	UFUNCTION(BlueprintCallable, Category = GameProgressManagement)
		void SetMonsterSpawner(AMonsterSpawnServiceActor * pointer);

	UFUNCTION(BlueprintCallable, Category = GameProgressManagement)
		APickupSpawnServiceActor* GetPickupSpawner();

	UFUNCTION(BlueprintCallable, Category = GameProgressManagement)
		void SetPickupSpawner(APickupSpawnServiceActor * pointer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		AMonsterSpawnServiceActor* MonsterSpawnServiceActor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		APickupSpawnServiceActor* PickupSpawnServiceActor;

	UFUNCTION(BlueprintCallable, Category = "GameProgressManagement")
		bool SpawnTrackableMonster(TSubclassOf<AEnemyBase> monster, FVector location);

	UFUNCTION(BlueprintCallable, Category = "GameProgressManagement")
		bool SpawnTrackableMonsterWithPickup(TSubclassOf<AEnemyBase> monster, FVector location, TSubclassOf<APickUpBase> pickup);

	UFUNCTION(BlueprintCallable, Category = "GameProgressManagement")
		void SpawnTrackablePickup(TSubclassOf<AActor> actor, FVector location);

	UFUNCTION()
		void MarkAsAnUnique(FName name);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
