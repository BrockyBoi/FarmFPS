// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

// Generated
#include "CraftingData.generated.h"

USTRUCT(BlueprintType)
struct FCraftingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input", BlueprintReadOnly)
	TArray<FModifiedResourceValue> RequiredResources;

	UPROPERTY(EditAnywhere, Category = "Output", BlueprintReadOnly)
	TArray<FModifiedResourceValue> ResourceProducts;
};