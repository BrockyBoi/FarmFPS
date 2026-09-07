// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "TutorialSaveGameData.generated.h"

USTRUCT(BlueprintType)
struct FTutorialSaveGameData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool ShouldShowTutorials;
};
