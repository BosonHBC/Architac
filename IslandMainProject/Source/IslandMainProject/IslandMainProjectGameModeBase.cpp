// Fill out your copyright notice in the Description page of Project Settings.


#include "IslandMainProjectGameModeBase.h"
#include "Public/VenturePawn.h"
#include "Public/CameraControlComponent.h"
#include "Public/InventoryComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Public/Building/Pawns/BuildingPawn.h"
#include "Public/Building/Actors/WorldBuildingGrid.h"
#include "Public/Building/Components/Core/BuildingTechTreeComponent.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/SkyEnemy.h"
#include "Enemy/OceanEnemy.h"
#include "Enemy/SkyAIController.h"
#include "Enemy/OceanAIController.h"
#include "Building/Actors/BlockSet.h"
#include "EngineUtils.h"
#include "CircleTrail.h"
#include "Libary/AILibary.h"
	/* ----Useful HEAD
#include "AncientWorldPlayerController.h"

#include "BuildingSynchronization.h"
*/
AIslandMainProjectGameModeBase::AIslandMainProjectGameModeBase()
{
	m_inBuildingMode = false;
	m_buildingTechTree = CreateDefaultSubobject<UBuildingTechTreeComponent>(TEXT("Building Tech Tree"));
}

AGameProgressDirectorActor* AIslandMainProjectGameModeBase::GetDirector()
{
	return Director;
}

void AIslandMainProjectGameModeBase::SetDirector(AGameProgressDirectorActor* pointer)
{
	Director = pointer;
}

void AIslandMainProjectGameModeBase::GoToExploreMode()
{
	if (this->m_CurrentMode != EModeEnum::ME_Exploration) {
		// Switch to exploration mode
		this->m_CurrentMode = EModeEnum::ME_Exploration;

		if (this->m_cachedVenturePawn == nullptr) {
			this->m_cachedVenturePawn = GetWorld()->SpawnActor<AVenturePawn>(m_venturePawnClass);
		}

		if (this->m_cachedBuildingPawn != nullptr) {
			this->m_cachedBuildingPawn->ExitBuildingMode();
		}

		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		controller->SetViewTargetWithBlend(this->m_cachedVenturePawn, 2.0f, EViewTargetBlendFunction::VTBlend_Linear, 0, false);
		controller->UnPossess();
		//controller->SetViewTargetWithBlend(this->m_cachedVenturePawn, 2.0f, EViewTargetBlendFunction::VTBlend_Linear, 0, false);
		controller->Possess(this->m_cachedVenturePawn);

		UE_LOG(LogTemp, Log, TEXT("Go to Exploration Mode"));
	}
}

void AIslandMainProjectGameModeBase::GoToBuildingMode(AWorldBuildingGrid * targetWorldGrid)
{
	if (this->m_CurrentMode != EModeEnum::ME_Building)
	{

		if (!targetWorldGrid)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not switch to Building Mode. Building Target Null"));
			return;
		}

		if (this->m_cachedBuildingPawn == nullptr)
		{
			FVector location = targetWorldGrid->GetActorLocation();
			FActorSpawnParameters spawnInfo;
			this->m_cachedBuildingPawn = GetWorld()->SpawnActor<ABuildingPawn>(m_buildingPawnClass, location, FRotator::ZeroRotator, spawnInfo);
			this->m_cachedBuildingPawn->SetTargetWorldAndInitialize(targetWorldGrid);
		}
		else
		{
			this->m_cachedBuildingPawn->SetTargetWorldAndInitialize(targetWorldGrid);
		}

		if (!this->m_cachedBuildingPawn)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not switch to Building Mode. Building Pawn NULL"));
			return;
		}

		if (this->m_cachedBuildingPawn && targetWorldGrid)
		{
			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			controller->UnPossess();
			//controller->SetViewTargetWithBlend(this->m_cachedBuildingPawn, 2.0f, EViewTargetBlendFunction::VTBlend_Linear, 0, false);
			controller->Possess(this->m_cachedBuildingPawn);

			this->m_cachedBuildingPawn->EnterBuildingMode();

			// Switch to building mode
			this->m_CurrentMode = EModeEnum::ME_Building;
			UE_LOG(LogTemp, Log, TEXT("Go to Building Mode"));
		}
	}
}

UBuildingTechTreeComponent* AIslandMainProjectGameModeBase::GetBuildingTechTree()
{
	return this->m_buildingTechTree;
}

void AIslandMainProjectGameModeBase::RegisterCachedBuildingPawn(ABuildingPawn * pawn)
{
	m_cachedBuildingPawn = pawn;
}

void AIslandMainProjectGameModeBase::GiveMeEverything() const
{
	if (m_cachedVenturePawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Venture Pawn Not Found"));
		return;
	}


	if (m_InventoryItemDB == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Item DB Not Found"));
		return;
	}


	TArray<FName> rowNames = m_InventoryItemDB->GetRowNames();
	UInventoryComponent* inventory = m_cachedVenturePawn->GetInventoryComponent();
	for (FName name : rowNames)
	{
		FInventoryItem* item = m_InventoryItemDB->FindRow<FInventoryItem>(name, "");

		int current = inventory->GetItemAmount(item->ItemID);
		inventory->AddItem(item->ItemID, (item->MaxStack/2) - current);
	}


}

bool AIslandMainProjectGameModeBase::SendAttackRequestToBuilding(ABlockSet* i_building, ASkyEnemy* i_enemy)
{
	if (i_building == nullptr || i_enemy == nullptr) return false;

	// = &m_enemyAttackingBuildingMap[i_building];

	if (!m_enemyAttackingBuildingMap.Contains(i_building)) {
		// this is a empty list, which means that this building is not registered as a target yet
		TArray<ASkyEnemy*> tempList;
		tempList.Add(i_enemy);
		i_building->GetOnGetDestroyDelegate()->AddDynamic(this, &AIslandMainProjectGameModeBase::TellEnemyToGoBackToRoutineWhenBuildingIsDestroyed);

		m_enemyAttackingBuildingMap.Add(i_building, tempList);

		// Request attack
		ASkyAIController* aiCtrl = Cast<ASkyAIController>(i_enemy->GetController());
		aiCtrl->SetAttackTarget(i_building);
	}
	else {

		if (!m_enemyAttackingBuildingMap[i_building].Contains(i_enemy)) {
			m_enemyAttackingBuildingMap[i_building].Add(i_enemy);

			// Request attack
			ASkyAIController* aiCtrl = Cast<ASkyAIController>(i_enemy->GetController());
			if (aiCtrl)
				aiCtrl->SetAttackTarget(i_building);
			else
				return false;
		}
		else {
			return false;
		}
	}

	return true;
}

bool AIslandMainProjectGameModeBase::SendAttackRequestToPlayer(AVenturePawn* i_player, AOceanEnemy* i_enemy)
{
	if (i_player == nullptr) return false;

	AOceanAIController* aiCtrl = Cast<AOceanAIController>(i_enemy->GetController());
	if (aiCtrl)
		aiCtrl->SetAttackTarget(i_player);

	return false;
}

void AIslandMainProjectGameModeBase::TellEnemyToGoBackToRoutineWhenBuildingIsDestroyed(ABlockSet* i_building)
{
	TArray<ASkyEnemy*> tempList = m_enemyAttackingBuildingMap[i_building];
	for (auto itr : tempList)
	{
		ASkyAIController* aiCtrl = Cast<ASkyAIController>(itr->GetController());
		aiCtrl->CleanUpAttackRequest();
	}
}

void AIslandMainProjectGameModeBase::SpawnNewSkyEnemy()
{
	if (m_skyEnemyList.Num() > 40) return;

	ACircleTrail* theTrail = nullptr;
	ETrailHeightLevel skyLevel = ETrailHeightLevel::THL_Air;
	int rnd = 0;
	int assetInt = 0;
	while (theTrail == nullptr)
	{
		rnd = FMath::RandRange(0, m_enemyMovingTrailList.Num() - 1);
		ACircleTrail* tempTrail = m_enemyMovingTrailList[rnd];
		if (tempTrail->CircleTrailLevel == skyLevel)
			theTrail = tempTrail;
		assetInt++;
		if (assetInt > 100) break;
	}

	if (theTrail == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Did not find a proper circle trail in the map right now! Can't spawn a sky enemy"));
		return;
	}
	// spawn a fat bird
	AEnemyBase* enemyBase = GetWorld()->SpawnActor<AEnemyBase>(EnemyBaseClasses[1], theTrail->GetRandomPointInCircle(), FRotator::ZeroRotator);
	if (enemyBase )
		m_skyEnemyList.AddUnique(Cast<ASkyEnemy>(enemyBase));
	else {
		SpawnNewSkyEnemy();
	}

	UE_LOG(LogTemp, Log, TEXT("Current Sky enemy count: %d"), m_skyEnemyList.Num());
}

void AIslandMainProjectGameModeBase::SpawnNewOceanEnemy()
{
	if (m_oceanEnemyList.Num() > 20) return;

	ACircleTrail* theTrail = nullptr;
	ETrailHeightLevel skyLevel = ETrailHeightLevel::THL_ShallowOcean;
	int rnd = 0;
	int assetInt = 0;
	while (theTrail == nullptr)
	{
		rnd = FMath::RandRange(0, m_enemyMovingTrailList.Num() - 1);
		ACircleTrail* tempTrail = m_enemyMovingTrailList[rnd];
		if (tempTrail->CircleTrailLevel == skyLevel)
			theTrail = tempTrail;
		assetInt++;
		if (assetInt > 100) break;
	}

	if (theTrail == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Did not find a proper circle trail in the map right now! Can't spawn a ocean enemy"));
		return;
	}
	// spawn a fat bird
	AEnemyBase* enemyBase = GetWorld()->SpawnActor<AEnemyBase>(EnemyBaseClasses[0], theTrail->GetRandomPointInCircle(), FRotator::ZeroRotator);
	
	if (enemyBase )
		m_oceanEnemyList.Add(Cast<AOceanEnemy>(enemyBase));
	else
		SpawnNewOceanEnemy();

	UE_LOG(LogTemp, Log, TEXT("Current Ocrean enemy count: %d"), m_oceanEnemyList.Num());

}

void AIslandMainProjectGameModeBase::RemoveSkyEnemyRef(ASkyEnemy* i_enemy)
{
	m_skyEnemyList.Remove(i_enemy);
}

void AIslandMainProjectGameModeBase::RemoveOceanEnemyRef(AOceanEnemy* i_enemy)
{
	m_oceanEnemyList.Remove(i_enemy);
}

void AIslandMainProjectGameModeBase::InitializeMovingTrailList()
{
	for (TObjectIterator<ACircleTrail> Itr; Itr; ++Itr)
	{
		m_enemyMovingTrailList.Add(*Itr);
	}

	// find all enemies and store them to list too

	for (TObjectIterator<ASkyEnemy> Itr; Itr; ++Itr)
	{
		m_skyEnemyList.Add(*Itr);
	}
	for (TObjectIterator<AOceanEnemy> Itr; Itr; ++Itr)
	{
		m_oceanEnemyList.Add(*Itr);
	}
}

void AIslandMainProjectGameModeBase::StartPlay()
{

	m_CurrentMode = EModeEnum::ME_Exploration;

	m_cachedVenturePawn = Cast<AVenturePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// try find the world grid
	for (TActorIterator<AWorldBuildingGrid> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		m_worldBuildingGrid = Cast<AWorldBuildingGrid>(*ActorItr);
		if (m_worldBuildingGrid) break;
	}
	if (!m_worldBuildingGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldBuildingGrid is found!"));
		return;
	}

	//UWorld* world = GetWorld();
	//if (world)
	//{
	//	FActorSpawnParameters spawnParams;
	//	AGameProgressDirectorActor* director = world->SpawnActor<AGameProgressDirectorActor>(AGameProgressDirectorActor::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), spawnParams);
	//	Director = director;
	//}

	InitializeMovingTrailList();

	Super::StartPlay();
}
