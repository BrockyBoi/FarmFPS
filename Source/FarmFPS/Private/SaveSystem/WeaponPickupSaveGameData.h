// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "WeaponPickupSaveGameData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponPickupSaveGameData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool HasBeenPickedUp;

	UPROPERTY()
	FGameplayTag WeaponTypeTag;
};
