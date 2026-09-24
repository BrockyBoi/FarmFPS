// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradePurchaceLocation.h"

// Brock
#include "Managers/FarmFPSUtilities.h"
#include "Managers/PerkManager.h"

void UUpgradePurchaceLocation::BeginPlay()
{
	Super::BeginPlay();
}

FString UUpgradePurchaceLocation::GetPerkDescriptionString() const
{
	FString upgradeName = _modifierUpgrade.GetTagName().ToString();
	upgradeName = upgradeName.Replace(TEXT("PerkModifierType."), TEXT(""));

	FString perkValueString = _perkUpgradeAmount.GetDescriptionText();

	UPerkManager* perkManager = UFarmFPSUtilities::GetPlayerPerkManager(this);
	float beforeValue = 0;
	float afterValue = 0;
	if (ensure(IsValid(perkManager)) && ensure(_baseValueRowData.GetValue().Modifiers != FGameplayTagContainer::EmptyContainer))
	{
		FPerkData currentPerkData = perkManager->GetPerkData(_modifierUpgrade);
		beforeValue = currentPerkData.GetModifiedValue(_baseValueRowData.GetValue().GetBaseValue());
		
		FPerkData modifiedPerkData = FPerkData(currentPerkData.AdditiveValue + _perkUpgradeAmount.AdditiveValue, currentPerkData.MultiplicativeValue + _perkUpgradeAmount.MultiplicativeValue);
		afterValue = modifiedPerkData.GetModifiedValue(_baseValueRowData.GetValue().GetBaseValue());
	}
	FString change = FString::Printf(TEXT("\n%f -> %f"), beforeValue, afterValue);
	FString finalString = upgradeName.Append(perkValueString).Append(change);

	return finalString;
}

void UUpgradePurchaceLocation::OnPurchaseSuccess(UPerkManager* perkManager, UResourceInventory* inventory)
{
	if (ensure(IsValid(perkManager)))
	{
		perkManager->ModifyPerkData(_modifierUpgrade, _perkUpgradeAmount);
	}

	Super::OnPurchaseSuccess(perkManager, inventory);
}

