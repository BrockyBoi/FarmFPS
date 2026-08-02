// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradePurchaceLocation.h"

// Brock
#include "Managers/PerkManager.h"

void UUpgradePurchaceLocation::OnPurchaseSuccess(UPerkManager* perkManager, UResourceInventory* inventory)
{
	perkManager->ModifyPerkData(_modifierUpgrade, _perkUpgradeAmount);

	Super::OnPurchaseSuccess(perkManager, inventory);
}

