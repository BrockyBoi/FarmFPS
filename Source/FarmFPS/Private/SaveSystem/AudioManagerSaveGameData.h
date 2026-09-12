// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "AudioManagerSaveGameData.generated.h"

USTRUCT(BlueprintType)
struct FAudioManagerSaveGameData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float FXAudioMultiplier;

	UPROPERTY()
	float MusicAudioMultiplier;

	UPROPERTY()
	float MasterAudioMultiplier;
};
