// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/TradeOffUpgradeManager.h"

// Brock
#include "DayNightCycleManager.h"
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

	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycleManager)))
	{
		dayNightCycleManager->OnWaitingForTradeOff.AddUObject(this, &UTradeOffUpgradeManager::OnTradeOffDayStateReached);
	}
}

void UTradeOffUpgradeManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycleManager))
	{
		dayNightCycleManager->OnWaitingForTradeOff.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UTradeOffUpgradeManager::OnTradeOffDayStateReached()
{
	GenerateTradeOff();
}

void UTradeOffUpgradeManager::IntializeTradeOffs()
{
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

void UTradeOffUpgradeManager::OnTradeOffAccepted()
{
	UPerkManager* perkManager = UFarmFPSUtilities::GetPlayerPerkManager(this);
	if (ensure(IsValid(perkManager)))
	{
		perkManager->ModifyPerkData(_currentTradeOff.PerkType, _currentTradeOff.PerkData);
		perkManager->ModifyPerkData(PerkModifierTypeTags::DailyBreadIncreaseAmount, _currentTradeOff.BreadIncreaseAmount);
		_currentTradeOff = FTradeOffData();
	}

	if (OnTradeOffAnyInput.IsBound())
	{
		OnTradeOffAnyInput.Broadcast();
	}

	if (OnTradeOffAcceptedInput.IsBound())
	{
		OnTradeOffAcceptedInput.Broadcast();
	}
}

void UTradeOffUpgradeManager::OnTradeOffDeclined()
{
	_currentTradeOff = FTradeOffData();
	if (OnTradeOffDeclinedInput.IsBound())
	{
		OnTradeOffDeclinedInput.Broadcast();
	}

	if (OnTradeOffAnyInput.IsBound())
	{
		OnTradeOffAnyInput.Broadcast();
	}
}

void UTradeOffUpgradeManager::GenerateTradeOff()
{
	if (_rarityChancePercentages.IsEmpty())
	{
		return;
	}

	if (_possibilities.IsEmpty())
	{
		IntializeTradeOffs();
		if (!ensure(!_possibilities.IsEmpty()))
		{
			return;
		}
	}

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
	if (!ensure(possibleUpgrades.Num() > 0))
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
	OnTradeOffCreated.Broadcast(_currentTradeOff);

	FTimerHandle timerHandle;
	//GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UTradeOffUpgradeManager::OnTradeOffDeclined, .5f, false);
}
