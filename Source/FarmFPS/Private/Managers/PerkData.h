// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"

// Generated
#include "PerkData.generated.h"

USTRUCT(BlueprintType)
struct FPerkData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AdditiveValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MultiplicativeValue;

	FPerkData()
	{
		AdditiveValue = 0.f;
		MultiplicativeValue = 1.f;
	}

	FPerkData(float additive, float multiplicative)
	{
		AdditiveValue = additive;
		MultiplicativeValue = multiplicative;
	}

	float GetModifiedValue(float baseValue)
	{
		return (baseValue * MultiplicativeValue) + (baseValue + AdditiveValue);
	}

	FString GetDescriptionText() const
	{
		FString perkValueString = TEXT("");
		if (!FMath::IsNearlyEqual(MultiplicativeValue, 1.f))
		{
			perkValueString = FString::Printf(TEXT(" times %f"), MultiplicativeValue);
		}
		else if (!FMath::IsNearlyZero(AdditiveValue))
		{
			perkValueString = AdditiveValue > 0 ? FString::Printf(TEXT(" plus %f"), AdditiveValue) : FString::Printf(TEXT(" minus %f"), AdditiveValue);
		}

		return perkValueString;
	}
};