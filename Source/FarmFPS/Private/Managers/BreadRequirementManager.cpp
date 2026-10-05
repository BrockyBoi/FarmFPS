// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadRequirementManager.h"

// Brock
#include "DayNightCycleManager.h"
#include "FarmFPSCharacter.h"
#include "FarmFPSUtilities.h"
#include "Managers/PerkManager.h"
#include "Managers/PerkModifierTypeTag.h"
#include "Resources/ResourceInventory.h"
#include "SaveSystem/SaveGameManager.h"

UBreadRequirementManager::UBreadRequirementManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBreadRequirementManager::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycleManager)))
	{
		if (dayNightCycleManager->IsDay())
		{
			OnDayBegin();
		}

		dayNightCycleManager->OnDayBegin.AddUObject(this, &UBreadRequirementManager::OnDayBegin);
		dayNightCycleManager->OnDayEnd.AddUObject(this, &UBreadRequirementManager::OnDayEnd);
	}

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGameManager)))
	{
		saveGameManager->OnLoadGameData.AddUObject(this, &UBreadRequirementManager::OnGameLoaded);
	}
}

void UBreadRequirementManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycleManager))
	{
		dayNightCycleManager->OnDayBegin.RemoveAll(this);
		dayNightCycleManager->OnDayEnd.RemoveAll(this);
	}

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGameManager))
	{
		saveGameManager->OnLoadGameData.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UBreadRequirementManager::SellBread(int breadAmount)
{
	_currentBreadSold += breadAmount;
	OnBreadSold.Broadcast(_currentBreadSold);
	if (!_metRequirementForDay && HasSoldBreadNeeded())
	{
		RequirementsMet();
	}
}

FBreadRequirementManagerSaveGameData UBreadRequirementManager::GetBreadRequriementSaveGameData() const
{
	FBreadRequirementManagerSaveGameData saveData;
	saveData.ConsecutiveDaysMetRequirement = _consecutiveDaysSoldBreadRequirement;

	return saveData;
}

void UBreadRequirementManager::OnGameLoaded(UFarmFPSSaveGame* saveGame)
{
	if (ensure(saveGame))
	{
		_consecutiveDaysSoldBreadRequirement = saveGame->GetBreadRequirmentSaveData().ConsecutiveDaysMetRequirement;
	}
}

void UBreadRequirementManager::OnDayBegin()
{
	_currentBreadSold = 0;
}

void UBreadRequirementManager::OnDayEnd()
{
	if (!HasSoldBreadNeeded())
	{
		DayFailed();
	}
}

void UBreadRequirementManager::RequirementsMet()
{
	_consecutiveDaysSoldBreadRequirement++;
	OnRequirementsMet.Broadcast();
	_metRequirementForDay = true;

	UPerkManager* perkManager = UFarmFPSUtilities::GetPlayerPerkManager(this);
	if (ensure(IsValid(perkManager)))
	{
		perkManager->SetPerkData(PerkModifierTypeTag::BonusDailyBreadRewardModifier, FPerkData(0, 1 + (_bonusMultiplierPerDaySold.GetValue().GetBaseValue() * _consecutiveDaysSoldBreadRequirement)));
	}
}

void UBreadRequirementManager::DayFailed()
{
	_consecutiveDaysSoldBreadRequirement = 0;
	OnDayFailed.Broadcast();
	_currentBreadSold = 0;

	UPerkManager* perkManager = UFarmFPSUtilities::GetPlayerPerkManager(this);
	if (ensure(IsValid(perkManager)))
	{
		perkManager->SetPerkData(PerkModifierTypeTag::BonusDailyBreadRewardModifier, FPerkData(0, 0));
	}
}
