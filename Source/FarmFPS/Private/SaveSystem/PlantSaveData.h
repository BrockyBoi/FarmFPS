// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"

// Generated
#include "PlantSaveData.generated.h"

USTRUCT(BlueprintType)
struct FPlantSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlantName;

	UPROPERTY()
	bool IsEnabled;
};
