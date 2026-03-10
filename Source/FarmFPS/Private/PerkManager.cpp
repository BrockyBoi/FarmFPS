// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkManager.h"

UPerkManager::UPerkManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const FPerkData UPerkManager::GetPerkData(EPerkModifiers perkType) const
{
	return _activePerks.Contains(perkType) ? _activePerks[perkType] : FPerkData();
}

void UPerkManager::ModifyAdditiveValue(EPerkModifiers perkType, float valueChange)
{
	_activePerks.FindOrAdd(perkType).AdditiveValue += valueChange;
}

void UPerkManager::ModifyMultiplicativeValue(EPerkModifiers perkType, float valueToMultiplyBy)
{
	_activePerks.FindOrAdd(perkType).MultiplicativeValue *= valueToMultiplyBy;
}

void UPerkManager::ModifyPerkData(EPerkModifiers perkType, const FPerkData& perkDataChange)
{
	FPerkData& perkData = _activePerks.FindOrAdd(perkType);
	perkData.AdditiveValue += perkDataChange.AdditiveValue;
	perkData.MultiplicativeValue *= perkDataChange.MultiplicativeValue;
}

float UPerkManager::ModifyValueByPerks(EPerkModifiers perkType, float valueToModify) const
{
	return (valueToModify + GetPerkData(perkType).AdditiveValue) * GetPerkData(perkType).MultiplicativeValue;
}

void UPerkManager::BeginPlay()
{
	Super::BeginPlay();
}

