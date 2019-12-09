// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingLibrary.h"
#include "BuildingTechTreeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UBuildingTechTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildingTechTreeComponent();

	UFUNCTION(BlueprintCallable, Category = "Building")
		TArray<FBlockSetData> GetPossibleBlockSets() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
private :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Block Sets", meta = (AllowPrivateAccess = "true"))
		TArray<FBlockSetData> m_possibleBuildingBlockSets;

		
};
