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
struct FARMFPS_API FModifiedIntValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "PerkModifier."))
	FGameplayTagContainer Modifiers;

	FModifiedIntValue()
	{
		BaseValue = 0;
	}

	FModifiedIntValue(int value)
	{
		BaseValue = value;
	}

	int GetModifiedValue(const UObject* WorldContextObject) const
	{
		return UFarmFPSUtilities::GetModifiedValueByPlayerPerks(WorldContextObject, Modifiers, BaseValue);
	}

	const int GetBaseValue() const
	{
		return BaseValue;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int BaseValue;
};

USTRUCT(BlueprintType)
struct FARMFPS_API FModifiedFloatValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (Categories = "PerkModifier."), BlueprintReadOnly)
	FGameplayTagContainer Modifiers;
	
	FModifiedFloatValue()
	{
		BaseValue = 0.f;
	}

	FModifiedFloatValue(float value)
	{
		BaseValue = value;
	}

	float GetModifiedValue(const UObject* WorldContextObject) const
	{
		return UFarmFPSUtilities::GetModifiedValueByPlayerPerks(WorldContextObject, Modifiers, BaseValue);
	}

	const float GetBaseValue() const
	{
		return BaseValue;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue;
};


USTRUCT(BlueprintType)
struct FARMFPS_API FModifiedResourceValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (Categories = "ResourceType."), BlueprintReadOnly)
	FGameplayTag ResourceTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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
