// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "TradeOffData.generated.h"

UENUM(BlueprintType)
enum class EPerkRarity : uint8
{
	Common,
	Rare,
	Epic,
	Legendary
};

USTRUCT(BlueprintType)
struct FTradeOffData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EPerkRarity Rarity;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag PerkType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerkData PerkData;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int BreadIncreaseAmount;

	FTradeOffData()
	{
		Rarity = EPerkRarity::Common;
		PerkType = FGameplayTag::EmptyTag;
		PerkData = FPerkData();
		BreadIncreaseAmount = 0;
	}

	FTradeOffData(EPerkRarity rarity, FGameplayTag perkType, FPerkData& perkData, int breadIncreaseAmount)
	{
		Rarity = rarity;
		PerkType = perkType;
		PerkData = perkData;
		BreadIncreaseAmount = breadIncreaseAmount;
	}
};
