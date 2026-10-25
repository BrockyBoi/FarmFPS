// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/PerkData.h"
#include "ShooterWeapon.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "PlayerSaveData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag WeaponType;

	UPROPERTY()
	EWeaponFireType FireType;
};

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int MoneyCount;

	UPROPERTY()
	TArray<FGameplayTag> PlayerUnlockedPerks;

	UPROPERTY()
	TArray<FPerkData> PlayerPerks;

	UPROPERTY()
	TArray<FWeaponData> UnlockedWeapons;
};