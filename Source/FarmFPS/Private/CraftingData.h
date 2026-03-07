// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ResourceData.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "CraftingData.generated.h"

USTRUCT(BlueprintType)
struct FCraftingData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EResourceType ResourceProductType;

	UPROPERTY(EditDefaultsOnly)
	TMap<EResourceType, uint16> RequiredResources;
};
