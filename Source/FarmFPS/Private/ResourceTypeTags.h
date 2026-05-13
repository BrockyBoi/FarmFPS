#pragma once

#include "GameplayTagContainer.h"

namespace ResourceTypeTags
{
	static const FGameplayTag None = FGameplayTag::RequestGameplayTag(FName("None"));

	static const FGameplayTag Bread = FGameplayTag::RequestGameplayTag(FName("ResourceType.Bread"));
	static const FGameplayTag Wheat = FGameplayTag::RequestGameplayTag(FName("ResourceType.Wheat"));
	static const FGameplayTag WaterMelon = FGameplayTag::RequestGameplayTag(FName("ResourceType.WaterMelon"));
	static const FGameplayTag SunFlower = FGameplayTag::RequestGameplayTag(FName("ResourceType.SunFlower"));
	static const FGameplayTag Money = FGameplayTag::RequestGameplayTag(FName("ResourceType.Money"));

	static const FGameplayTag Water = FGameplayTag::RequestGameplayTag(FName("CropResourceType.Water"));
	static const FGameplayTag Light = FGameplayTag::RequestGameplayTag(FName("CropResourceType.Light"));
	static const FGameplayTag Love = FGameplayTag::RequestGameplayTag(FName("CropResourceType.Love"));
};

