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
		return (baseValue * MultiplicativeValue) + AdditiveValue;
	}

	FString GetDescriptionText() const
	{
		FString perkValueString = TEXT("");
		FString numberString;

		FNumberFormattingOptions Opts;
		Opts.SetMinimumFractionalDigits(0);
		Opts.SetMaximumFractionalDigits(2);
		if (!FMath::IsNearlyEqual(MultiplicativeValue, 1.f))
		{
			numberString = FText::AsNumber(MultiplicativeValue, &Opts).ToString();
			perkValueString = FString::Printf(TEXT(" * %s"), *numberString);
		}
		else if (!FMath::IsNearlyZero(AdditiveValue))
		{
			numberString = FText::AsNumber(AdditiveValue, &Opts).ToString();
			perkValueString = AdditiveValue > 0 ? FString::Printf(TEXT(" + %s"), *numberString) : FString::Printf(TEXT(" - %s"), *numberString);
		}

		return perkValueString;
	}
};