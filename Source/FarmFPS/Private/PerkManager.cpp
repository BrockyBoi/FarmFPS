// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkManager.h"

UPerkManager::UPerkManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const FPerkData UPerkManager::GetPerkData(const FGameplayTag& perkTag) const
{
	return _activePerks.Contains(perkTag) ? _activePerks[perkTag] : FPerkData();
}

void UPerkManager::ModifyAdditiveValue(const FGameplayTag& perkTag, float valueChange)
{
	_activePerks.FindOrAdd(perkTag).AdditiveValue += valueChange;
}

void UPerkManager::ModifyMultiplicativeValue(const FGameplayTag& perkTag, float valueToMultiplyBy)
{
	_activePerks.FindOrAdd(perkTag).MultiplicativeValue *= valueToMultiplyBy;
}

void UPerkManager::ModifyPerkData(const FGameplayTag& perkTag, const FPerkData& perkDataChange)
{
	FPerkData& perkData = _activePerks.FindOrAdd(perkTag);
	perkData.AdditiveValue += perkDataChange.AdditiveValue;
	perkData.MultiplicativeValue *= perkDataChange.MultiplicativeValue;
}

float UPerkManager::ModifyValueByPerks(const FGameplayTag& perkTag, float valueToModify) const
{
	return (valueToModify + GetPerkData(perkTag).AdditiveValue) * GetPerkData(perkTag).MultiplicativeValue;
}

float UPerkManager::ModifyValueByPerks(const FGameplayTagContainer& perkTags, float valueToModify) const
{
	for (const FGameplayTag& perkTag : perkTags)
	{
		valueToModify += GetPerkData(perkTag).AdditiveValue;
	}

	for (const FGameplayTag& perkTag : perkTags)
	{
		valueToModify *= GetPerkData(perkTag).MultiplicativeValue;
	}
	return valueToModify;
}

void UPerkManager::BeginPlay()
{
	Super::BeginPlay();
}

