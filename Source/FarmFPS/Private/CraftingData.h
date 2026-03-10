// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"
#include "ResourceData.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "CraftingData.generated.h"

USTRUCT(BlueprintType)
struct FModifiedResourceValue
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EResourceType ResourceType;

	UPROPERTY(EditDefaultsOnly)
	EPerkModifiers Modifier;
};

// Provide a hash function so this strongly-typed enum can be used as a key in UE containers (TSet/TMap)
FORCEINLINE uint32 GetTypeHash(const FModifiedResourceValue Value)
{
	return ::GetTypeHash(static_cast<uint8>(Value.ResourceType) + static_cast<uint8>(Value.Modifier));
}

USTRUCT(BlueprintType)
struct FCraftingData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<FModifiedResourceValue, uint16> RequiredResources;

	UPROPERTY(EditDefaultsOnly, Category = "Output")
	FModifiedResourceValue ResourceProduct;

	UPROPERTY(EditDefaultsOnly, Category = "Output")
	uint16 ResourceProductCount;
};
