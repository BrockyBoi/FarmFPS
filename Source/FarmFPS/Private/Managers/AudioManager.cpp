// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/AudioManager.h"

// Brock
#include "Managers/FarmFPSUtilities.h"
#include "SaveSystem/SaveGameManager.h"

// UE
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UAudioManager::UAudioManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAudioManager::BeginPlay()
{
	Super::BeginPlay();

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGameManager)))
	{
		saveGameManager->OnLoadGameSettingsData.AddUObject(this, &UAudioManager::OnGameLoaded);
	}
}

void UAudioManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGameManager))
	{
		saveGameManager->OnLoadGameSettingsData.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UAudioManager::OnGameLoaded(UFarmFPSSaveGameSettings* saveGame)
{
	if (saveGame)
	{
		SetFXAudioMultiplier(saveGame->GetAudioSaveGameData().FXAudioMultiplier, true);
		SetMusicAudioMultiplier(saveGame->GetAudioSaveGameData().MusicAudioMultiplier, true);
		SetMasterAudioMultiplier(saveGame->GetAudioSaveGameData().MasterAudioMultiplier, true);
	}
}

FAudioManagerSaveGameData UAudioManager::GetSaveGameData()
{
	FAudioManagerSaveGameData audioSaveData;
	audioSaveData.FXAudioMultiplier = _fxAudioMultiplier;
	audioSaveData.MusicAudioMultiplier = _musicAudioMultiplier;
	audioSaveData.MasterAudioMultiplier = _masterAudioMultiplier;

	return audioSaveData;
}

void UAudioManager::SetFXAudioMultiplier(float multiplier, bool setFromLoad)
{
	_fxAudioMultiplier = FMath::Clamp(multiplier, 0, 1);
	for (TObjectPtr<UAudioComponent> audioComponent : _currentlyPlayingFXAudioComponents)
	{
		if (ensure(IsValid(audioComponent)))
		{
			audioComponent->SetVolumeMultiplier(GetModifiedFXMultiplier());
		}
	}

	if (!setFromLoad)
	{
		USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
		if (ensure(IsValid(saveGameManager)))
		{
			saveGameManager->SaveGameSettings();
		}
	}
}

void UAudioManager::SetMusicAudioMultiplier(float multiplier, bool setFromLoad)
{
	_musicAudioMultiplier = FMath::Clamp(multiplier, 0, 1);
	TArray<TObjectPtr<UAudioComponent>> currentlyPlayingMusicAudioComponents;
	_currentlyPlayingMusicAudioComponents.GenerateValueArray(currentlyPlayingMusicAudioComponents);
	for (TObjectPtr<UAudioComponent> audioComponent : currentlyPlayingMusicAudioComponents)
	{
		if (ensure(IsValid(audioComponent)))
		{
			audioComponent->SetVolumeMultiplier(GetModifiedMusicMultiplier());
		}
	}

	if (!setFromLoad)
	{
		USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
		if (ensure(IsValid(saveGameManager)))
		{
			saveGameManager->SaveGameSettings();
		}
	}
}

void UAudioManager::SetMasterAudioMultiplier(float multiplier, bool setFromLoad)
{
	_masterAudioMultiplier = FMath::Clamp(multiplier, 0, 1);
	SetFXAudioMultiplier(_fxAudioMultiplier, setFromLoad);
	SetMusicAudioMultiplier(_musicAudioMultiplier, setFromLoad);

	if (!setFromLoad)
	{
		USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
		if (ensure(IsValid(saveGameManager)))
		{
			saveGameManager->SaveGameSettings();
		}
	}
}

void UAudioManager::AddMusicAudioComponentToMap(TObjectPtr<USoundBase> musicClip, TObjectPtr<UAudioComponent> audioComponent)
{
	if (ensure(IsValid(musicClip)) && ensure(IsValid(audioComponent)) && !_currentlyPlayingMusicAudioComponents.Contains(musicClip))
	{
		_currentlyPlayingMusicAudioComponents.Add(musicClip, audioComponent);
	}
}

void UAudioManager::AddFXAudioComponentToArray(TObjectPtr<UAudioComponent> audioComponent)
{
	if (ensure(IsValid(audioComponent)))
	{
		_currentlyPlayingFXAudioComponents.Add(audioComponent);
	}
}

void UAudioManager::OnFXAudioComponentStopPlaying()
{
	TArray<TObjectPtr<UAudioComponent>> currentlyPlayingAudio = _currentlyPlayingFXAudioComponents;
	for (auto audioComponent : currentlyPlayingAudio)
	{
		if (ensure(IsValid(audioComponent)) && !audioComponent->IsPlaying())
		{
			audioComponent->OnAudioFinished.RemoveAll(this);
			_currentlyPlayingFXAudioComponents.Remove(audioComponent);
		}
	}
}

TObjectPtr<UAudioComponent> UAudioManager::GetMusicAudioComponent(TObjectPtr<USoundBase> musicClip)
{
	if (ensure(IsValid(musicClip)) && _currentlyPlayingMusicAudioComponents.Contains(musicClip))
	{
		return _currentlyPlayingMusicAudioComponents[musicClip];
	}

	return SpawnMusic2D(this, musicClip);
}

void UAudioManager::SpawnSoundAtLocation(UObject* worldContext, TObjectPtr<USoundBase> soundClip, const FVector& location)
{
	UAudioManager* audioManager = UFarmFPSUtilities::GetAudioManager(worldContext);
	if (ensure(IsValid(audioManager)) && ensure(IsValid(soundClip)))
	{
		auto audioComponent = UGameplayStatics::SpawnSoundAtLocation(worldContext, soundClip, location, FRotator::ZeroRotator, audioManager->GetModifiedFXMultiplier());
		if (ensure(IsValid(audioComponent)))
		{
			audioComponent->bAutoDestroy = true;
			audioComponent->OnAudioFinished.AddDynamic(audioManager, &UAudioManager::OnFXAudioComponentStopPlaying);
			audioManager->AddFXAudioComponentToArray(audioComponent);
		}
	}
}

void UAudioManager::SpawnSound2D(UObject* worldContext, TObjectPtr<USoundBase> soundClip)
{
	UAudioManager* audioManager = UFarmFPSUtilities::GetAudioManager(worldContext);
	if (ensure(IsValid(audioManager)) && ensure(IsValid(soundClip)))
	{
		auto audioComponent = UGameplayStatics::SpawnSound2D(worldContext, soundClip, audioManager->GetModifiedFXMultiplier());
		if (ensure(IsValid(audioComponent)))
		{
			audioComponent->bAutoDestroy = true;
			audioComponent->OnAudioFinished.AddDynamic(audioManager, &UAudioManager::OnFXAudioComponentStopPlaying);
			audioManager->AddFXAudioComponentToArray(audioComponent);
		}
	}
}

TObjectPtr<UAudioComponent> UAudioManager::SpawnMusic2D(UObject* worldContext, TObjectPtr<USoundBase> musicClip)
{
	UAudioManager* audioManager = UFarmFPSUtilities::GetAudioManager(worldContext);
	if (ensure(IsValid(audioManager)) && ensure(IsValid(musicClip)))
	{
		TObjectPtr<UAudioComponent> musicAudioComponent = UGameplayStatics::SpawnSound2D(worldContext, musicClip, audioManager->GetModifiedMusicMultiplier());
		if (ensure(IsValid(musicAudioComponent)))
		{
			musicAudioComponent->bAutoDestroy = false;
			musicAudioComponent->Play();

			audioManager->AddMusicAudioComponentToMap(musicClip, musicAudioComponent);

			return musicAudioComponent;
		}
	}

	return nullptr;
}
