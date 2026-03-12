#pragma once

#include "GameplayTagContainer.h"

namespace PerkModifierTypeTags
{
	static const FGameplayTag MoveSpeed = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MoveSpeed"));
	static const FGameplayTag FireRate = FGameplayTag::RequestGameplayTag(FName("PerkModifier.FireRate"));
	static const FGameplayTag WaterEfficacy = FGameplayTag::RequestGameplayTag(FName("PerkModifier.WaterEfficacy"));
	static const FGameplayTag LightEfficacy = FGameplayTag::RequestGameplayTag(FName("PerkModifier.LightEfficacy"));
	static const FGameplayTag MoreCropYield = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MoreCropYield"));
	static const FGameplayTag BreadPrice = FGameplayTag::RequestGameplayTag(FName("PerkModifier.BreadPrice"));
	static const FGameplayTag WheatNeededForBread = FGameplayTag::RequestGameplayTag(FName("PerkModifier.WheatNeededForBread"));
	static const FGameplayTag MoneyNeededForUpgrades = FGameplayTag::RequestGameplayTag(FName("PerkModifier.MoneyNeededForUpgrades"));
};

