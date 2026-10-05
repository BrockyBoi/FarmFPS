// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkManager.h"

#include "PerkModifierTypeTag.h"

UPerkManager::UPerkManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPerkManager::BeginPlay()
{
	Super::BeginPlay();
}

const FPerkData UPerkManager::GetPerkData(const FGameplayTag& perkTag) const
{
	return _activePerks.Contains(perkTag) ? _activePerks[perkTag] : FPerkData();
}

const TMap<FGameplayTag, FPerkData>& UPerkManager::GetAllActivePerks() const
{
	return _activePerks;
}

void UPerkManager::SetAllPerksFromSave(const TArray<FGameplayTag>& perkTags, const TArray<FPerkData>& perkData)
{
	_activePerks.Empty();
	for (int i = 0; i < perkTags.Num(); ++i)
	{
		if (perkData.IsValidIndex(i))
		{
			_activePerks.Add(perkTags[i], perkData[i]);
		}
	}
}

void UPerkManager::SetAllActivePerks(const TMap<FGameplayTag, FPerkData>& newPerks)
{
	_activePerks = newPerks;
}

void UPerkManager::ModifyAdditiveValue(const FGameplayTag& perkTag, float valueChange)
{
	if (!ensure(IsValidTag(perkTag)))
	{
		return;
	}

	_activePerks.FindOrAdd(perkTag).AdditiveValue += valueChange;

	OnPerkLevelChange.Broadcast(perkTag, GetPerkData(perkTag));
}

void UPerkManager::ModifyMultiplicativeValue(const FGameplayTag& perkTag, float valueToMultiplyBy)
{
	if (!ensure(IsValidTag(perkTag)))
	{
		return;
	}

	_activePerks.FindOrAdd(perkTag).MultiplicativeValue *= valueToMultiplyBy;

	OnPerkLevelChange.Broadcast(perkTag, GetPerkData(perkTag));
}

void UPerkManager::ModifyPerkData(const FGameplayTag& perkTag, const FPerkData& perkDataChange)
{
	if (!ensure(IsValidTag(perkTag)))
	{
		return;
	}

	FPerkData& perkData = _activePerks.FindOrAdd(perkTag);
	perkData.AdditiveValue += perkDataChange.AdditiveValue;
	perkData.MultiplicativeValue *= perkDataChange.MultiplicativeValue;

	OnPerkLevelChange.Broadcast(perkTag, GetPerkData(perkTag));
}

float UPerkManager::ModifyValueByPerks(const FGameplayTag& perkTag, float valueToModify) const
{
	if (!ensure(IsValidTag(perkTag)))
	{
		return 0.f;
	}

	return valueToModify * GetPerkData(perkTag).MultiplicativeValue + GetPerkData(perkTag).AdditiveValue;
}

float UPerkManager::ModifyValueByPerks(const FGameplayTagContainer& perkTags, float valueToModify) const
{
	float startingValue = valueToModify;

	for (const FGameplayTag& perkTag : perkTags)
	{
		if (!ensure(IsValidTag(perkTag)))
		{
			continue;
		}

		valueToModify *= GetPerkData(perkTag).MultiplicativeValue;
	}

	for (const FGameplayTag& perkTag : perkTags)
	{
		if (!ensure(IsValidTag(perkTag)))
		{
			continue;
		}

		valueToModify += GetPerkData(perkTag).AdditiveValue;
	}

	return valueToModify;
}

void UPerkManager::SetPerkData(const FGameplayTag& perkTag, const FPerkData& newPerkData)
{
	if (!ensure(IsValidTag(perkTag)))
	{
		return;
	}

	FPerkData& perkData = _activePerks.FindOrAdd(perkTag);
	perkData.AdditiveValue = newPerkData.AdditiveValue;
	perkData.MultiplicativeValue = newPerkData.MultiplicativeValue;
}

bool UPerkManager::IsValidTag(const FGameplayTag& tag) const
{
	return tag.GetTagName().ToString().Contains(PerkModifierTypeTag::PerkModifierBaseTag.GetTag().ToString());
}


