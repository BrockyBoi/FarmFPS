// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/PerkData.h"
#include "Misc/CachedModifiableValueDataTableRowHandle.h"
#include "PurchaseLocation.h"

// UE
#include "GameplayTagContainer.h"

// Generated
#include "UpgradePurchaceLocation.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UUpgradePurchaceLocation : public UPurchaseLocation
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	FString GetPerkDescriptionString() const;

	virtual void OnPurchaseSuccess(class UPerkManager* perkManager, class UResourceInventory* inventory) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade", meta = (Categories = "PerkModifier."))
	FGameplayTag _modifierUpgrade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade")
	FPerkData _perkUpgradeAmount;

	UPROPERTY(EditAnywhere, Category = "Tunables")
	FCachedModifiableValueDataTableRowHandle _baseValueRowData;
};
