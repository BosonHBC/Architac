// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "NPCPathFollowingComponent.generated.h"

/**
 * Custom Navgation System for enemy and NPC
 */
UCLASS()
class ISLANDMAINPROJECT_API UNPCPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()
public:
	UNPCPathFollowingComponent();

protected:
	virtual void BeginPlay() override;
	
	//-----------
	// Movement Segment
	//------------

	/** follow current path segment */
	void FollowPathSegment(float DeltaTime) override;
	/** sets variables related to current move segment */
	void SetMoveSegment(int32 SegmentStartIndex) override;
	/** check state of path following, update move segment if needed */
	void UpdatePathSegment() override;

	//-----------
	// Navgation Data
	//------------

	/** Get the main Nav Data*/
	const class ANavigationData* GetMainNavData(FNavigationSystem::ECreateIfEmpty CreateNewIfNoneFound);
	/** Get Agent Nav Data*/
	const class ANavigationData* GetNavData();
	/** Crash Protection */
	bool TileIsValid(const class ARecastNavMesh* NavMesh, int32 TileIndex);

	/** Get All Navgation Poly*/
	bool GetAllNavPolys(TArray<NavNodeRef>& Polys);
};
