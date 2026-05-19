// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadRequirementManager.h"

UBreadRequirementManager::UBreadRequirementManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBreadRequirementManager::BeginPlay()
{
	Super::BeginPlay();
}

void UBreadRequirementManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UBreadRequirementManager::SellBread(int breadAmount)
{
	_currentBreadSold += breadAmount;
	if (HasSoldBreadNeeded())
	{
		OnRequirementsMet();
	}
}

void UBreadRequirementManager::OnDayBegin()
{
	_currentBreadSold = 0;

	if (_isFirstDay)
	{
		_breadRequiredForCurrentDay = _startingBreadRequired.GetModifiedValue(this);
		_isFirstDay = false;
	}
	else
	{
		_breadRequiredForCurrentDay = FMath::CeilToInt(_breadRequiredForCurrentDay * _dailyMultiplier.GetModifiedValue(this));
	}
}

void UBreadRequirementManager::OnDayEnd()
{
	if (!HasSoldBreadNeeded())
	{
		OnDayFailed();
	}
}

void UBreadRequirementManager::OnBreadSold()
{}

void UBreadRequirementManager::OnRequirementsMet()
{}

void UBreadRequirementManager::OnDayFailed()
{}
