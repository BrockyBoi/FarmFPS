// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "FarmFPSUtilities.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "ModifiedValueData.generated.h"

USTRUCT(BlueprintType)
struct FModifiedIntValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "PerkModifier."))
	FGameplayTagContainer Modifiers;

	int GetModifiedValue(const UObject* WorldContextObject) const
	{
		return FarmFPSUtilities::GetModifiedValueByPlayerPerks(WorldContextObject, Modifiers, BaseValue);
	}

	const int GetBaseValue() const
	{
		return BaseValue;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int BaseValue;
};

USTRUCT(BlueprintType)
struct FModifiedFloatValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "PerkModifier."), BlueprintReadOnly)
	FGameplayTagContainer Modifiers;

	float GetModifiedValue(const UObject* WorldContextObject) const
	{
		return FarmFPSUtilities::GetModifiedValueByPlayerPerks(WorldContextObject, Modifiers, BaseValue);
	}

	const float GetBaseValue() const
	{
		return BaseValue;
	}

	FModifiedFloatValue& operator=(float value)
	{
		BaseValue = value;

		return *this;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseValue;
};


USTRUCT(BlueprintType)
struct FModifiedResourceValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."), BlueprintReadOnly)
	FGameplayTag ResourceTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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
