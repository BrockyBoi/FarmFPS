// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkModifierTypeTag.h"

namespace PerkModifierTypeTag
{
	// Base Player Movement
	UE_DEFINE_GAMEPLAY_TAG(JumpCount, "PerkModifierType.JumpCount");
	UE_DEFINE_GAMEPLAY_TAG(JumpHeight, "PerkModifierType.JumpHeight");
	UE_DEFINE_GAMEPLAY_TAG(MoveSpeed, "PerkModifierType.MoveSpeed");

	// Player Components
	UE_DEFINE_GAMEPLAY_TAG(PlayerPickupColliderSize, "PerkModifierType.PlayerPickupColliderSize");

	// Weapon Modifiers
	UE_DEFINE_GAMEPLAY_TAG(FireRate, "PerkModifierType.FireRate");

	// Resource Modifiers
	UE_DEFINE_GAMEPLAY_TAG(BreadPrice, "PerkModifierType.BreadPrice");
	UE_DEFINE_GAMEPLAY_TAG(LightEfficacy, "PerkModifierType.LightEfficacy");
	UE_DEFINE_GAMEPLAY_TAG(MoreCropYield, "PerkModifierType.MoreCropYield");
	UE_DEFINE_GAMEPLAY_TAG(WaterEfficacy, "PerkModifierType.WaterEfficacy");
	UE_DEFINE_GAMEPLAY_TAG(WheatNeededForBread, "PerkModifierType.WheatNeededForBread");

	// Money Related
	UE_DEFINE_GAMEPLAY_TAG(MoneyNeededForUpgrades, "PerkModifierType.MoneyNeededForUpgrades");

	// Customers
	UE_DEFINE_GAMEPLAY_TAG(CustomerSpawnRateIncrease, "PerkModifierType.CustomerSpawnRateIncrease");
	UE_DEFINE_GAMEPLAY_TAG(MaxCustomerCount, "PerkModifierType.MaxCustomerCount");

	// Bread Related
	UE_DEFINE_GAMEPLAY_TAG(DailyBreadIncreaseAmount, "PerkModifierType.DailyBreadIncreaseAmount");
}
