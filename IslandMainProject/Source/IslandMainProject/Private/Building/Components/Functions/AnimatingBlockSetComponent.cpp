// Fill out your copyright notice in the Description page of Project Settings.

#include "Functions/AnimatingBlockSetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

UAnimatingBlockSetComponent::UAnimatingBlockSetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UAnimatingBlockSetComponent::BeginPlay()
{
	Super::BeginPlay();

	this->GetOwner()->GetComponents<USkeletalMeshComponent>(this->m_skeletalMeshesToEnableAndDisable, true);
}
