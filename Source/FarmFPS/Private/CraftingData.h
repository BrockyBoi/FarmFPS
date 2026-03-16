// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "FarmFPSUtilities.h"
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "CraftingData.generated.h"

USTRUCT(BlueprintType)
struct FModifiedIntValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "PerkModifier."))
	FGameplayTagContainer Modifiers;

	int GetModifiedValue(const UObject* WorldContextObject) const
	{
		int value = BaseValue;
		FarmFPSUtilities::GetModifiedValueByPlayerPerks(WorldContextObject, Modifiers, value);
		return value;
	}

	const int GetBaseValue() const
	{
		return BaseValue;
	}

private:
	UPROPERTY(EditDefaultsOnly)
	int BaseValue;
};

USTRUCT(BlueprintType)
struct FModifiedFloatValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "PerkModifier."))
	FGameplayTagContainer Modifiers;

	float GetModifiedValue(const UObject* WorldContextObject) const
	{
		float value = BaseValue;
		FarmFPSUtilities::GetModifiedValueByPlayerPerks(WorldContextObject, Modifiers, value);
		return value;
	}

	const float GetBaseValue() const
	{
		return BaseValue;
	}

private:
	UPROPERTY(EditDefaultsOnly)
	float BaseValue;
};


USTRUCT(BlueprintType)
struct FModifiedResourceValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."))
	FGameplayTag ResourceTag;

	UPROPERTY(EditDefaultsOnly)
	FModifiedIntValue ModifiedIntValue;
};

// Provide a hash function so this strongly-typed enum can be used as a key in UE containers (TSet/TMap)
FORCEINLINE uint32 GetTypeHash(const FModifiedIntValue Value)
{
	int hash = GetTypeHash(Value.GetBaseValue());
	for (const FGameplayTag& tag : Value.Modifiers.GetGameplayTagArray())
	{
		hash += GetTypeHash(tag);
	}
	return hash;
}

// Provide a hash function so this strongly-typed enum can be used as a key in UE containers (TSet/TMap)
FORCEINLINE uint32 GetTypeHash(const FModifiedFloatValue Value)
{
	int hash = GetTypeHash(Value.GetBaseValue());
	for (const FGameplayTag& tag : Value.Modifiers.GetGameplayTagArray())
	{
		hash += GetTypeHash(tag);
	}
	return hash;
}

// Provide a hash function so this strongly-typed enum can be used as a key in UE containers (TSet/TMap)
FORCEINLINE uint32 GetTypeHash(const FModifiedResourceValue Value)
{
	return GetTypeHash(Value.ResourceTag) + GetTypeHash(Value.ModifiedIntValue);
}

USTRUCT(BlueprintType)
struct FCraftingData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<FModifiedResourceValue> RequiredResources;

	UPROPERTY(EditDefaultsOnly, Category = "Output")
	TArray<FModifiedResourceValue> ResourceProducts;
};