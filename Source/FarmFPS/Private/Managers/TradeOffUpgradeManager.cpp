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

	if (ensure(IsValid(_tradeOffPossiblitiesTable)))
	{
		// Iterate through the internal row map directly
		for (auto It = _tradeOffPossiblitiesTable->GetRowMap().CreateConstIterator(); It; ++It)
		{
			FName RowName = It.Key();
			FTradeOffPossibility* row = reinterpret_cast<FTradeOffPossibility*>(It.Value());

			if (ensure(row))
			{
				_possibilities.Add(row->PerkType, *row);
			}
		}
	}
}

void UTradeOffUpgradeManager::OnUpgradeAccepted()
{
	UPerkManager* perkManager = FarmFPSUtilities::GetPlayerPerkManager(this);
	if (ensure(IsValid(perkManager)))
	{
		perkManager->ModifyPerkData(_currentTradeOff.PerkType, _currentTradeOff.PerkData);
		perkManager->ModifyPerkData(PerkModifierTypeTags::DailyBreadIncreaseAmount, _currentTradeOff.BreadIncreaseAmount);
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

	TArray<FGameplayTag> possibleUpgrades;
	_possibilities.GetKeys(possibleUpgrades);
	if (!ensure(possibleUpgrades.Num() <= 0))
	{
		return;
	}

	FGameplayTag perkType;
	do
	{
		perkType = possibleUpgrades[FMath::RandRange(0, _possibilities.Num() - 1)];
	}
	while (!_possibilities[perkType].PlayerPerkValues.Contains(rarity));

	_currentTradeOff = FTradeOffData(rarity, perkType, _possibilities[perkType].PlayerPerkValues[rarity], _breadIncreaseAmounts[rarity]);
}
