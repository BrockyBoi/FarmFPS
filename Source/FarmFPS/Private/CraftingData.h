// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"
#include "ResourceData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "CraftingData.generated.h"

USTRUCT(BlueprintType)
struct FModifiedResourceValue
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."))
	FGameplayTag ResourceTag;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "PerkModifier."))
	FGameplayTagContainer Modifiers;
};

// Provide a hash function so this strongly-typed enum can be used as a key in UE containers (TSet/TMap)
FORCEINLINE uint32 GetTypeHash(const FModifiedResourceValue Value)
{
	int hash = GetTypeHash(Value.ResourceTag);
	for (const FGameplayTag& tag : Value.Modifiers.GetGameplayTagArray())
	{
		hash += GetTypeHash(tag);
	}
	return hash;
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