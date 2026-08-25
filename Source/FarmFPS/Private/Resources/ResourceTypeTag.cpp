// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceTypeTag.h"

namespace ResourceTypeTag
{
	UE_DEFINE_GAMEPLAY_TAG(None, "None");

	UE_DEFINE_GAMEPLAY_TAG(Bread, "ResourceType.Bread");
	UE_DEFINE_GAMEPLAY_TAG(Wheat, "ResourceType.Wheat");
	UE_DEFINE_GAMEPLAY_TAG(WaterMelon, "ResourceType.WaterMelon");
	UE_DEFINE_GAMEPLAY_TAG(SunFlower, "ResourceType.SunFlower");
	UE_DEFINE_GAMEPLAY_TAG(Money, "ResourceType.Money");

	UE_DEFINE_GAMEPLAY_TAG(Water, "CropResourceType.Water");
	UE_DEFINE_GAMEPLAY_TAG(Light, "CropResourceType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Love, "CropResourceType.Love");
	UE_DEFINE_GAMEPLAY_TAG(Damage, "CropResourceType.Damage");
};
