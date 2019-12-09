// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/Components/CreatureLandPoint.h"

UCreatureLandPoint::UCreatureLandPoint()
{
	this->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}
