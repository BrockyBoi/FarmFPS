// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Misc/CachedModifiableValueDataTableRowHandle.h"
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "CropGrowthData.generated.h"

USTRUCT(BlueprintType)
struct FPlantGrowthData
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

	FPlantGrowthData()
	{
		StartingScaleSize = 0.5f;
		FinalScaleSize = 2.5f;
		WaterNeeded = 100;
		LightNeeded = 100;
		LoveNeeded = 100;
		CropHealth = 1;
	}
};
