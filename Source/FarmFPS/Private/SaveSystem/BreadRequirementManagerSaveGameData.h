// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "BreadRequirementManagerSaveGameData.generated.h"

USTRUCT(BlueprintType)
struct FBreadRequirementManagerSaveGameData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int DailyRequiredBread;
};
