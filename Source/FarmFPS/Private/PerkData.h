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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float AdditiveValue;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float MultiplicativeValue;

	FPerkData()
	{
		AdditiveValue = 0.f;
		MultiplicativeValue = 1.f;
	}
};

UENUM(BlueprintType)
enum class EPerkType : uint8
{
	MoveSpeed,
	FireRate,
	WaterEfficacy,
	LightEfficacy,
	MoreYield,
	None
};
