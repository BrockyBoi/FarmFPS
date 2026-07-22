// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "CropData.generated.h"

USTRUCT(BlueprintType)
struct FCropData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedIntValue WaterNeeded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedIntValue LightNeeded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedIntValue LoveNeeded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedIntValue CropHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StartingScaleSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FinalScaleSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedIntValue NumberOfPickupsToDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedIntValue NumberOfLoveSeedsToDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "ResourceType."))
	FGameplayTag ResourceType;

	FCropData()
	{
		StartingScaleSize = 0.5f;
		FinalScaleSize = 2.5f;
		WaterNeeded = 100;
		LightNeeded = 100;
		LoveNeeded = 100;
		CropHealth = 1;
		NumberOfPickupsToDrop = 3;
		NumberOfLoveSeedsToDrop = 1;
	}
};
