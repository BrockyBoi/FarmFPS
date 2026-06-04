// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "TradeOffData.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "TradeOffPossibility.generated.h"

USTRUCT(BlueprintType)
struct FTradeOffPossibility : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "PerkModifier"))
	FGameplayTag PerkType;

	UPROPERTY(EditDefaultsOnly)
	TMap<EPerkRarity, FPerkData> PlayerPerkValues;
};