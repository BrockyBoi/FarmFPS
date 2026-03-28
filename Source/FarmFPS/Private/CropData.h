// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	int WaterNeeded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int LightNeeded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StartingScaleSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FinalScaleSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 NumberOfPickupsToDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "ResourceType."))
	FGameplayTag ResourceType;

	FCropData()
	{
		StartingScaleSize = 0.5f;
		FinalScaleSize = 2.5f;
		WaterNeeded = 100;
		LightNeeded = 100;
		NumberOfPickupsToDrop = 3;
	}
};
