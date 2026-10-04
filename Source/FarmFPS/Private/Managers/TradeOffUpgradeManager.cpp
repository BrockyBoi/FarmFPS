// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/TradeOffUpgradeManager.h"

// Brock
#include "DayNightCycleManager.h"
#include "FarmFPSUtilities.h"
#include "PerkManager.h"
#include "PerkModifierTypeTag.h"

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

FString UTradeOffUpgradeManager::GetDescriptionString() const
{
	FString rarity = UEnum::GetValueAsString(_currentTradeOff.Rarity);
	rarity = rarity.Replace(TEXT("EPerkRarity::"), TEXT(""));
	FString upgradeName = _currentTradeOff.PerkType.GetTagName().ToString();
	upgradeName = upgradeName.Replace(TEXT("PerkModifier."), TEXT(""));
	FModifiedFloatValue value;
	if (ensure(IsValid(_baseValuesTable)))
	{
		// Iterate through the internal row map directly
		for (auto It = _baseValuesTable->GetRowMap().CreateConstIterator(); It; ++It)
		{
			FName RowName = It.Key();
			FBaseModifiableValueData* row = reinterpret_cast<FBaseModifiableValueData*>(It.Value());
			if (ensure(row))
			{
				bool hasModifiers = !row->BaseModifiedValue.Modifiers.IsEmpty();
				if (RowName == upgradeName || (hasModifiers && row->BaseModifiedValue.Modifiers.GetGameplayTagArray()[0] == _currentTradeOff.PerkType))
				{
					value = row->BaseModifiedValue;
					break;
				}
			}
		}
	}

	FNumberFormattingOptions Opts;
	Opts.SetMinimumFractionalDigits(0);
	Opts.SetMaximumFractionalDigits(2);

	UPerkManager* perkManager = UFarmFPSUtilities::GetPlayerPerkManager(this);
	float beforeValue = 0;
	float afterValue = 0;
	float beforeBreadValue = 0;
	float afterBreadValue = 0;
	if (ensure(IsValid(perkManager)))
	{
		FPerkData currentPerkData = perkManager->GetPerkData(_currentTradeOff.PerkType);
		beforeValue = currentPerkData.GetModifiedValue(value.GetBaseValue());

		FPerkData modifiedPerkData = FPerkData(currentPerkData.AdditiveValue + _currentTradeOff.PerkData.AdditiveValue, currentPerkData.MultiplicativeValue * _currentTradeOff.PerkData.MultiplicativeValue);
		afterValue = modifiedPerkData.GetModifiedValue(value.GetBaseValue());

		FPerkData currentBreadData = perkManager->GetPerkData(PerkModifierTypeTag::DailyBreadIncreaseAmount);
		beforeBreadValue = currentBreadData.GetModifiedValue(value.GetBaseValue());
		afterBreadValue = beforeBreadValue + _currentTradeOff.BreadIncreaseAmount.AdditiveValue;
	}
	FString change = FString::Printf(TEXT("\n%s -> %s"), *FText::AsNumber(beforeValue, &Opts).ToString(), *FText::AsNumber(afterValue, &Opts).ToString());
	FString breadChange = FString::Printf(TEXT("\n%s -> %s"), *FText::AsNumber(beforeBreadValue, &Opts).ToString(), *FText::AsNumber(afterBreadValue, &Opts).ToString());
	FString finalString = rarity.Append(TEXT("\n\n")).Append(upgradeName).Append(change).Append(TEXT("\nRequired Bread Per Day")).Append(breadChange);

	return finalString;
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
		perkManager->ModifyPerkData(PerkModifierTypeTag::DailyBreadIncreaseAmount, _currentTradeOff.BreadIncreaseAmount);
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
