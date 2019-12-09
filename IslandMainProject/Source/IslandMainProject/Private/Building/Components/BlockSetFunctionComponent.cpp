// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockSetFunctionComponent.h"
#include "BlockSet.h"
// Sets default values for this component's properties
UBlockSetFunctionComponent::UBlockSetFunctionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UBlockSetFunctionComponent::EnableBlockSetComponent()
{
	this->m_activeStatus = EComponentActiveStatus::ENABLED;
	OnBlockSetFunctionEnable_Implementation();
}

void UBlockSetFunctionComponent::DisableBlockSetComponent()
{
	this->m_activeStatus = EComponentActiveStatus::DISABLED;
	OnBlockSetFunctionDisable_Implementation();
}

void UBlockSetFunctionComponent::OnBlockSetFunctionEnable_Implementation()
{
	return;
}


void UBlockSetFunctionComponent::OnBlockSetFunctionDisable_Implementation()
{
	return;
}

// Called when the game starts
void UBlockSetFunctionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBlockSetFunctionComponent::SetBuildingSetParent(ABlockSet* i_pointer)
{
	this->m_buildingSetParent = i_pointer;
}