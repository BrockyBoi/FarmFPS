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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FModifiedIntValue WaterNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FModifiedIntValue LightNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FModifiedIntValue LoveNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FModifiedIntValue CropHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StartingScaleSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FinalScaleSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FModifiedIntValue NumberOfPickupsToDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FModifiedIntValue NumberOfLoveSeedsToDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "ResourceType."))
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
