// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace PerkModifierTypeTag
{
	 // Base Player Movement
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(JumpCount);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(JumpHeight);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoveSpeed);

	// Player Components
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerPickupColliderSize);

	// Weapon Modifiers
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireRate);

	// Resource Modifiers
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BreadPrice);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(LightEfficacy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoreCropYield);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WaterEfficacy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(WheatNeededForBread);

	// Money Related
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MoneyNeededForUpgrades);

	// Customers
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CustomerSpawnRateIncrease);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxCustomerCount);

	// Bread Related
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DailyBreadIncreaseAmount);
}