#pragma once

#include "GameplayTagContainer.h"

namespace PerkModifierTypeTags
{
	// Base Player Movement
	static const FGameplayTag JumpCount = FGameplayTag::RequestGameplayTag(FName("PerkModifier.JumpCount"));
	static const FGameplayTag JumpHeight = FGameplayTag::RequestGameplayTag(FName("PerkModifier.JumpHeight"));
	static const FGameplayTag MoveSpeed = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MoveSpeed"));

	// Player Components
	static const FGameplayTag PlayerPickupColliderSize = FGameplayTag::RequestGameplayTag(FName("PerkModifier.PlayerPickupColliderSize"));

	// Weapon Modifiers
	static const FGameplayTag FireRate = FGameplayTag::RequestGameplayTag(FName("PerkModifier.FireRate"));

	// Resource Modifiers
	static const FGameplayTag BreadPrice = FGameplayTag::RequestGameplayTag(FName("PerkModifier.BreadPrice"));
	static const FGameplayTag LightEfficacy = FGameplayTag::RequestGameplayTag(FName("PerkModifier.LightEfficacy"));
	static const FGameplayTag MoreCropYield = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MoreCropYield"));
	static const FGameplayTag WaterEfficacy = FGameplayTag::RequestGameplayTag(FName("PerkModifier.WaterEfficacy"));
	static const FGameplayTag WheatNeededForBread = FGameplayTag::RequestGameplayTag(FName("PerkModifier.WheatNeededForBread"));

	// Money Related
	static const FGameplayTag MoneyNeededForUpgrades = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MoneyNeededForUpgrades"));

	// Customers
	static const FGameplayTag CustomerSpawnRateIncrease = FGameplayTag::RequestGameplayTag(FName("PerkModifier.CustomerSpawnRateIncrease"));
	static const FGameplayTag MaxCustomerCount = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MaxCustomerCount"));
};

