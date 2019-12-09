// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioManagerComponent.h"
#include "Engine/Classes/Engine/DataTable.h"
#include "Engine/Classes/Sound/SoundBase.h"
#include "Engine/Classes/Sound/SoundCue.h"
#include "Public/StaticLibrary.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Engine/Engine.h"
#include "Engine/Classes/Components/AudioComponent.h"
// Sets default values for this component's properties
UAudioManagerComponent::UAudioManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


void UAudioManagerComponent::UpdateAudioSourceTable(UDataTable* sourceTable)
{
	AudioSourceTable = sourceTable;
	GenerateSourceMapping();
}

void UAudioManagerComponent::GenerateSourceMapping()
{
	if (AudioSourceTable == nullptr) return;
	auto RowNames = AudioSourceTable->GetRowNames();
	//AudioSourceTable->GetRowMap()
	USoundBase* DefaultSound = nullptr;
	for (auto it : m_channels)
	{
		it.Value->Stop();
	}
	m_channels.Empty();
	m_soundMap.Empty();
	for (auto it : AudioSourceTable->GetRowMap())
	{
		m_soundMap.Add(it.Key, reinterpret_cast<FSoundEffectRow*>(it.Value));
		if (m_soundMap[it.Key]->Sound != nullptr)
		{
			DefaultSound = m_soundMap[it.Key]->Sound;
		}
	} 
	
	const auto world = GetOwner()->GetWorld();
	for (auto it : m_soundMap)
	{
		if (!m_channels.Contains(it.Value->Channel) || m_channels[it.Value->Channel] == nullptr)
		{
			m_channels.Add(it.Value->Channel, UGameplayStatics::SpawnSoundAtLocation(world, DefaultSound, FVector(0, 0, 0)));
			if (m_channels[it.Value->Channel] != nullptr)
				m_channels[it.Value->Channel]->Stop();
		}
	}
}

void UAudioManagerComponent::PlayOnceIndividual(FName SoundName, FVector Location)
{
	if (m_soundMap.Contains(SoundName) && m_soundMap[SoundName]->Sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetOwner()->GetWorld(), m_soundMap[SoundName]->Sound, Location);
	}
}

void UAudioManagerComponent::Play(FName SoundName, FVector Location)
{
	/*if (m_soundMap.Contains(SoundName) && m_soundMap[SoundName]->Sound != nullptr)
	{
		m_channels[m_soundMap[SoundName]->Channel]->SetSound(m_soundMap[SoundName]->Sound);
		m_channels[m_soundMap[SoundName]->Channel]->SetWorldLocation(Location);	
		m_channels[m_soundMap[SoundName]->Channel]->Play();
	}*/
	PlayOnceIndividual(SoundName, Location);
}

void UAudioManagerComponent::Stop(FName SoundName)
{
	if (m_soundMap.Contains(SoundName) && m_channels[m_soundMap[SoundName]->Channel] && m_channels[m_soundMap[SoundName]->Channel]->Sound->GetName() != "None"  && m_channels[m_soundMap[SoundName]->Channel]->Sound == m_channels[m_soundMap[SoundName]->Channel]->Sound)
	{
		m_channels[m_soundMap[SoundName]->Channel]->Stop();
		// set start point to 0
	}
}

void UAudioManagerComponent::StopByChannel(FName ChannelName)
{
	if (m_channels.Contains(ChannelName))
	{
		m_channels[ChannelName]->Stop();
		// set start point to 0
	}
}

// Called when the game starts
void UAudioManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GenerateSourceMapping();
}


// Called every frame
void UAudioManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

