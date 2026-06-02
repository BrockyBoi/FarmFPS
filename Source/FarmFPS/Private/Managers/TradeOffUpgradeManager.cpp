// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/TradeOffUpgradeManager.h"

// Brock
#include "FarmFPSUtilities.h"
#include "PerkManager.h"
#include "PerkModifierTypeTags.h"

UTradeOffUpgradeManager::UTradeOffUpgradeManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTradeOffUpgradeManager::BeginPlay()
{
	Super::BeginPlay();
}

void UTradeOffUpgradeManager::OnUpgradeAccepted()
{
	UPerkManager* perkManager = FarmFPSUtilities::GetPlayerPerkManager(this);
	if (ensure(IsValid(perkManager)))
	{
		perkManager->ModifyAdditiveValue(PerkModifierTypeTags::DailyBreadIncreaseAmount, _currentTradeOff.BreadIncreaseAmount);
		_currentTradeOff = FTradeOffData();
	}
}

void UTradeOffUpgradeManager::OnUpgradeDeclined()
{
	_currentTradeOff = FTradeOffData();
}

void UTradeOffUpgradeManager::GenerateTradeOffs()
{
	float percentage = FMath::FRand();
	EPerkRarity rarity = EPerkRarity::Common;
	FGameplayTag perkType = _potentialUpgradeTypes.GetGameplayTagArray()[FMath::RandRange(0, _potentialUpgradeTypes.Num() - 1)];
	if (percentage < _rarityChancePercentages[EPerkRarity::Legendary].GetModifiedValue(this))
	{
		rarity = EPerkRarity::Legendary;
	}
	else if (percentage < _rarityChancePercentages[EPerkRarity::Epic].GetModifiedValue(this))
	{
		rarity = EPerkRarity::Epic;
	}
	else if (percentage < _rarityChancePercentages[EPerkRarity::Rare].GetModifiedValue(this))
	{
		rarity = EPerkRarity::Rare;
	}

	_currentTradeOff = FTradeOffData(rarity, perkType, _playerPerkValues[rarity], _breadIncreaseAmounts[rarity]);
}
