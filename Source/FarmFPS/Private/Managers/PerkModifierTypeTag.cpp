// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkModifierTypeTag.h"

namespace PerkModifierTypeTag
{
	// Base Player Movement
	UE_DEFINE_GAMEPLAY_TAG(JumpCount, "PerkModifier.JumpCount");
	UE_DEFINE_GAMEPLAY_TAG(JumpHeight, "PerkModifier.JumpHeight");
	UE_DEFINE_GAMEPLAY_TAG(MoveSpeed, "PerkModifier.MoveSpeed");

	// Player Components
	UE_DEFINE_GAMEPLAY_TAG(PlayerPickupColliderSize, "PerkModifier.PlayerPickupColliderSize");

	// Weapon Modifiers
	UE_DEFINE_GAMEPLAY_TAG(FireRate, "PerkModifier.FireRate");

	// Resource Modifiers
	UE_DEFINE_GAMEPLAY_TAG(BreadPrice, "PerkModifier.BreadPrice");
	UE_DEFINE_GAMEPLAY_TAG(LightEfficacy, "PerkModifier.LightEfficacy");
	UE_DEFINE_GAMEPLAY_TAG(MoreCropYield, "PerkModifier.MoreCropYield");
	UE_DEFINE_GAMEPLAY_TAG(WaterEfficacy, "PerkModifier.WaterEfficacy");
	UE_DEFINE_GAMEPLAY_TAG(WheatNeededForBread, "PerkModifier.WheatNeededForBread");

	// Money Related
	UE_DEFINE_GAMEPLAY_TAG(MoneyNeededForUpgrades, "PerkModifier.MoneyNeededForUpgrades");

	// Customers
	UE_DEFINE_GAMEPLAY_TAG(CustomerSpawnRateIncrease, "PerkModifier.CustomerSpawnRateIncrease");
	UE_DEFINE_GAMEPLAY_TAG(MaxCustomerCount, "PerkModifier.MaxCustomerCount");

	// Bread Related
	UE_DEFINE_GAMEPLAY_TAG(DailyBreadIncreaseAmount, "PerkModifier.DailyBreadIncreaseAmount");
}
