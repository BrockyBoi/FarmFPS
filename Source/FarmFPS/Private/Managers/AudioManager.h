// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "SaveSystem/Saveable.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "AudioManager.generated.h"

class UAudioComponent;
class USoundBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UAudioManager : public UActorComponent, public ISaveableSettings
{
	GENERATED_BODY()

public:	
	UAudioManager();

	virtual void OnGameLoaded(UFarmFPSSaveGameSettings* saveGame);
	FAudioManagerSaveGameData GetSaveGameData();

	UFUNCTION(BlueprintCallable)
	void SetFXAudioMultiplier(float multiplier, bool setFromLoad = false);

	UFUNCTION(BlueprintCallable)
	void SetMusicAudioMultiplier(float multiplier, bool setFromLoad = false);

	UFUNCTION(BlueprintCallable)
	void SetMasterAudioMultiplier(float multiplier, bool setFromLoad = false);

	UFUNCTION(BlueprintPure)
	float GetFXAudioMultiplier() const { return _fxAudioMultiplier; }

	UFUNCTION(BlueprintPure)
	float GetMusicAudioMultiplier() const { return _musicAudioMultiplier; }

	UFUNCTION(BlueprintPure)
	float GetMasterAudioMultiplier() const { return _masterAudioMultiplier; }

	UFUNCTION(BlueprintPure)
	float GetModifiedFXMultiplier() const { return _masterAudioMultiplier * _fxAudioMultiplier; }

	UFUNCTION(BlueprintPure)
	float GetModifiedMusicMultiplier() const { return _masterAudioMultiplier * _musicAudioMultiplier; }

	TObjectPtr<UAudioComponent> GetMusicAudioComponent(TObjectPtr<USoundBase> musicClip);

	static void SpawnSoundAtLocation(UObject* worldContext, TObjectPtr<USoundBase> soundClip, const FVector& location);
	static void SpawnSound2D(UObject* worldContext, TObjectPtr<USoundBase> soundClip);
	static TObjectPtr<UAudioComponent> SpawnMusic2D(UObject* worldContext, TObjectPtr<USoundBase> musicClip);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	void AddMusicAudioComponentToMap(TObjectPtr<USoundBase> musicClip, TObjectPtr<UAudioComponent> audioComponent);
	void AddFXAudioComponentToArray(TObjectPtr<UAudioComponent> audioComponent);

	UFUNCTION()
	void OnFXAudioComponentStopPlaying();

	TArray<TObjectPtr<UAudioComponent>> _currentlyPlayingFXAudioComponents;
	TMap<TObjectPtr<USoundBase>, TObjectPtr<UAudioComponent>> _currentlyPlayingMusicAudioComponents;

	float _fxAudioMultiplier = 1.f;
	float _musicAudioMultiplier = 1.f;
	float _masterAudioMultiplier = 1.f;
};
