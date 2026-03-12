#pragma once

#include "GameplayTagContainer.h"

namespace ResourceTypeTags
{
	static const FGameplayTag Bread = FGameplayTag::RequestGameplayTag(FName("ResourceType.Bread"));
	static const FGameplayTag Wheat = FGameplayTag::RequestGameplayTag(FName("ResourceType.Wheat"));
	static const FGameplayTag Money = FGameplayTag::RequestGameplayTag(FName("ResourceType.Money"));
};

