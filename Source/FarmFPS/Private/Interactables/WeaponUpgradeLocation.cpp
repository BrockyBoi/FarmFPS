// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactables/WeaponUpgradeLocation.h"

// Brock
#include "Managers/FarmFPSUtilities.h"

// FarmFPS
#include "FarmFPSCharacter.h"
#include "ShooterCharacter.h"

void UWeaponUpgradeLocation::BeginPlay()
{
	Super::BeginPlay();
}

FString UWeaponUpgradeLocation::GetPerkDescriptionString() const
{
	AActor* owner = GetOwner();
	AShooterWeapon* weapon = GetWeapon();
	if (!ensure(IsValid(owner)) || !ensure(IsValid(weapon)))
	{
		return TEXT("Invalid");
	}

	FString weaponName = _weaponUpgradeData.WeaponToUpgrade.ToString();
	weaponName = weaponName.Replace(TEXT("WeaponType."), TEXT(""));

	FString currentFireRateType = UEnum::GetValueAsString(weapon->GetWeaponFireType());

	FNumberFormattingOptions Opts;
	Opts.SetMinimumFractionalDigits(0);
	Opts.SetMaximumFractionalDigits(2);

	FString change = FString::Printf(TEXT("\n%s -> %s"), *currentFireRateType, *UEnum::GetValueAsString(_weaponUpgradeData.FireType));
	FString finalString = weaponName.Append(change);

	return finalString;
}

void UWeaponUpgradeLocation::OnPurchaseSuccess(UPerkManager* perkManager, UResourceInventory* inventory)
{
	Super::OnPurchaseSuccess(perkManager, inventory);

	if (ensure(_shooterCharacter.IsValid()))
	{
		AShooterWeapon* weapon = GetWeapon();
		if (ensure(IsValid(weapon)))
		{
			weapon->SetWeaponFireType(_weaponUpgradeData.FireType);
		}
	}
}

AShooterWeapon* UWeaponUpgradeLocation::GetWeapon() const
{
	if (ensure(_shooterCharacter.IsValid()))
	{
		return _shooterCharacter->FindWeaponOfType(_weaponUpgradeData.WeaponToUpgrade);
	}

	return nullptr;
}
