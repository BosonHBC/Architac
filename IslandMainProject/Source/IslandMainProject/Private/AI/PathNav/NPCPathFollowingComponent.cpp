// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/AI/PathNav/NPCPathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
UNPCPathFollowingComponent::UNPCPathFollowingComponent()
{

}

void UNPCPathFollowingComponent::BeginPlay()
{
	Super::BeginPlay();

/*
	TArray<NavNodeRef> Polys;
	if (GetAllNavPolys(Polys)) {
		UE_LOG(LogTemp, Log, TEXT("Poly length: %d"), Polys.Num());
		const ANavigationData* navData = GetNavData();

	}*/
}

void UNPCPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	Super::FollowPathSegment(DeltaTime);
}

void UNPCPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);
}

void UNPCPathFollowingComponent::UpdatePathSegment()
{
	Super::UpdatePathSegment();
}


const ANavigationData* UNPCPathFollowingComponent::GetMainNavData(FNavigationSystem::ECreateIfEmpty CreateNewIfNoneFound)
{
	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if (!NavSys) return nullptr;
	return NavSys->GetDefaultNavDataInstance(CreateNewIfNoneFound);
}

const ANavigationData* UNPCPathFollowingComponent::GetNavData()
{
	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	const ANavigationData* NavData = NavSys->GetNavDataForProps(AgentProperties);
	if (NavData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR FINDING AGENT NAV DATA, USING MAIN NAV DATA"));
		NavData = GetMainNavData(FNavigationSystem::ECreateIfEmpty::Create);
	}

	return NavData;

}

bool UNPCPathFollowingComponent::TileIsValid(const ARecastNavMesh* NavMesh, int32 TileIndex)
{
	if (!NavMesh) return false;

	const FBox TileBounds = NavMesh->GetNavMeshTileBounds(TileIndex);
	return TileBounds.IsValid != 0;
}

bool UNPCPathFollowingComponent::GetAllNavPolys(TArray<NavNodeRef>& Polys)
{
	if (!MovementComp) return false;

	//Get agent Nav Data
	const ANavigationData* NavData = GetNavData();

	// Get Nav mesh
	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
	if (!NavMesh) return false;

	TArray<FNavPoly> EachPolys;

	for (int32 v = 0; v < NavMesh->GetNavMeshTilesCount(); ++v)
	{

		// CHECK IS VALID FIRST OR WILL CRASH!!! 
		// 256 entries but only few are valid!
		// use continue in case the valid polys are not stored sequentially
		if (!TileIsValid(NavMesh, v))
		{
			continue;
		}

		NavMesh->GetPolysInTile(v, EachPolys);
		
	}

	for (FNavPoly poly : EachPolys)
	{
		DrawDebugSphere(GetWorld(), poly.Center, 20, 12, FColor::Green, false, 999, (uint8)'\000', 2);
	}
	//Add them all!
	for (int32 v = 0; v < EachPolys.Num(); ++v)
	{
		Polys.Add(EachPolys[v].Ref);
	}
	return true;
}
