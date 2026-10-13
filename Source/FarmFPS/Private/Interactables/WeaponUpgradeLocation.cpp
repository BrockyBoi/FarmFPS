// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactables/WeaponUpgradeLocation.h"

// Brock
#include "Managers/FarmFPSUtilities.h"

// FarmFPS
#include "FarmFPSCharacter.h"
#include "ShooterCharacter.h"
#include "ShooterWeapon.h"

void UWeaponUpgradeLocation::BeginPlay()
{
	Super::BeginPlay();
}

FString UWeaponUpgradeLocation::GetPerkDescriptionString() const
{
	return FString();
}

void UWeaponUpgradeLocation::OnPurchaseSuccess(UPerkManager* perkManager, UResourceInventory* inventory)
{
	AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
	if (ensure(IsValid(shooterCharacter)))
	{
		AShooterWeapon* weapon = shooterCharacter->FindWeaponOfType(_weaponUpgradeData.WeaponToUpgrade);
		if (ensure(IsValid(weapon)))
		{
			if (_weaponUpgradeData.MakeShotgun)
			{
				weapon->SetIsShotgun(true);
			}
		}
	}
}
