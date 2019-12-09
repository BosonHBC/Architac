// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioManagerComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ISLANDMAINPROJECT_API UAudioManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAudioManagerComponent();

	UFUNCTION(BlueprintCallable)
		void UpdateAudioSourceTable(class UDataTable* sourceTable);

	UFUNCTION(BlueprintCallable)
		void Play(FName SoundName, FVector Location);

	UFUNCTION(BlueprintCallable)
		void PlayOnceIndividual(FName SoundName, FVector Location);


	UFUNCTION(BlueprintCallable)
		void StopByChannel(FName ChannelName);

	UFUNCTION(BlueprintCallable)
		void Stop(FName SoundName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UDataTable* AudioSourceTable;

	void GenerateSourceMapping();
	TMap<FName, struct FSoundEffectRow*> m_soundMap;
	TMap<FName, class UAudioComponent*> m_channels;
};
