// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadRequirementManager.h"

// Brock
#include "DayNightCycleManager.h"
#include "FarmFPSUtilities.h"

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
}

void UBreadRequirementManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycleManager))
	{
		dayNightCycleManager->OnDayBegin.RemoveAll(this);
		dayNightCycleManager->OnDayEnd.RemoveAll(this);
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

void UBreadRequirementManager::OnDayBegin()
{
	_currentBreadSold = 0;

	if (_isFirstDay)
	{
		_breadRequiredForCurrentDay = _startingBreadRequired;
		_isFirstDay = false;
	}
	else
	{
		_breadRequiredForCurrentDay += _dailyBreadIncreaseAmount.GetModifiedValue(this);
	}
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
}

void UBreadRequirementManager::DayFailed()
{
	_consecutiveDaysSoldBreadRequirement = 0;
	OnDayFailed.Broadcast();
	_breadRequiredForCurrentDay = 0;
	_currentBreadSold = 0;
}
