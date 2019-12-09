// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameProgressDirectorActor.h"
#include "IslandMainProjectGameModeBase.generated.h"

UENUM(BlueprintType)
enum class EModeEnum : uint8
{
	ME_Exploration 	UMETA(DisplayName = "Exploration"),
	ME_Building 	UMETA(DisplayName = "Building")
};


class AVenturePawn;
class AEnemyBase;
class ASkyEnemy;
class AOceanEnemy;
class ACircleTrail;

UCLASS(minimalapi)
class  AIslandMainProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AIslandMainProjectGameModeBase();

	class UDataTable* GetItemDB() const { return m_InventoryItemDB; }
	class UDataTable* GetBuildingBlockSetCostTable() const { return BuildingBlockSetCostTable; }
	class UDataTable* GetBuildingBlockSetPropertiesTable() const { return BuildingBlockSetPropertiesTable; }
	class UDataTable* GetToolDB() const { return m_toolDB; }
	class UDataTable* GetAudioSourceTable() const { return AudioSourceTable; }

	UFUNCTION(BlueprintCallable, Category = GameProgressManagement)
		AGameProgressDirectorActor* GetDirector();

	UFUNCTION(BlueprintCallable, Category = GameProgressManagement)
		void SetDirector(AGameProgressDirectorActor * pointer);

	UPROPERTY(EditDefaultsOnly, Category = "GameDirector")
		AGameProgressDirectorActor * Director;

	UFUNCTION(BlueprintCallable)
		AVenturePawn* GetVenturePawn() const { return m_cachedVenturePawn; }
	/* ----Useful HEAD
	TArray<TSubclassOf<class AAPToolBase>> GetSpawnToolList() const { return m_ToolClassesToSpawn; }
	*/


protected:
	/* ----Useful HEAD
	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
		TArray<TSubclassOf<class AAPToolBase>> m_ToolClassesToSpawn;
		*/
	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
		class UDataTable* m_InventoryItemDB;
	UPROPERTY(EditDefaultsOnly, Category = "BuildingSystem")
		class UDataTable* BuildingBlockSetCostTable;
	UPROPERTY(EditDefaultsOnly, Category = "BuildingSystem")
		class UDataTable* BuildingBlockSetPropertiesTable;
	UPROPERTY(EditDefaultsOnly, Category = "Tool")
		class UDataTable* m_toolDB;
	UPROPERTY(EditDefaultsOnly, Category = "AudioSystem")
		class UDataTable* AudioSourceTable;

public:
	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		FORCEINLINE bool InBuildingMode() const { return m_inBuildingMode; }

	UFUNCTION(BlueprintCallable, Category = "BuilidingSystem")
		void GoToExploreMode();

	UFUNCTION(BlueprintCallable, Category = "BuildingSystem")
		void GoToBuildingMode(class AWorldBuildingGrid* targetWorldGrid);

	UPROPERTY(VisibleAnywhere, Category = "Game Mode State")
		EModeEnum m_CurrentMode;

	class UBuildingTechTreeComponent* GetBuildingTechTree();

	UFUNCTION(BlueprintCallable, Category = "Building System")
		void RegisterCachedBuildingPawn(class ABuildingPawn * pawn);

	UFUNCTION(BlueprintCallable, Category = "Building System")
		AWorldBuildingGrid* GetWorldGridActor() const { return m_worldBuildingGrid; }


	UFUNCTION(Exec, Category = "ExecFunctions")
		void GiveMeEverything() const;

	// Handle enemy spawn and death
	//------------------------------------------
	/** For setting the enemy to go to attack player and building*/
	UFUNCTION(BlueprintCallable, Category = AI_Enemy)
		bool SendAttackRequestToBuilding(ABlockSet* i_building, ASkyEnemy* i_enemy);
	UFUNCTION(BlueprintCallable, Category = AI_Enemy)
		bool SendAttackRequestToPlayer(AVenturePawn* i_player, AOceanEnemy* i_enemy);
	UFUNCTION(BlueprintCallable, Category = AI_Enemy)
		void TellEnemyToGoBackToRoutineWhenBuildingIsDestroyed(ABlockSet* i_building);
	TMap<ABlockSet*, TArray<ASkyEnemy*>> m_enemyAttackingBuildingMap;


	/** For spawn new enemy when an enemy is dead or in time*/
	UPROPERTY(EditDefaultsOnly, Category = AI_Enemy)
		TArray<TSubclassOf<AEnemyBase>> EnemyBaseClasses;
	UFUNCTION(Exec, Category = "ExecFunctions")
		void SpawnNewSkyEnemy();
	UFUNCTION(Exec, Category = "ExecFunctions")
		void SpawnNewOceanEnemy();

	void RemoveSkyEnemyRef(ASkyEnemy* i_enemy);

	void RemoveOceanEnemyRef(AOceanEnemy* i_enemy);
protected:
	TArray<ACircleTrail*> m_enemyMovingTrailList;
	void InitializeMovingTrailList();

	// Stored the enemy reference here
	TArray<ASkyEnemy*> m_skyEnemyList;
	TArray<AOceanEnemy*> m_oceanEnemyList;

	//------------------------------------------
protected:
	bool m_inBuildingMode;

	UPROPERTY(EditDefaultsOnly, Category = "Exploring System")
		TSubclassOf<class AVenturePawn> m_venturePawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Building System")
		TSubclassOf<class ABuildingPawn> m_buildingPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Building System")
		class AWorldBuildingGrid* m_worldBuildingGrid;


	AVenturePawn* m_cachedVenturePawn;
	class ABuildingPawn* m_cachedBuildingPawn;

	virtual void StartPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building", meta = (AllowPrivateAccess = "true"))
	class UBuildingTechTreeComponent* m_buildingTechTree;

};
