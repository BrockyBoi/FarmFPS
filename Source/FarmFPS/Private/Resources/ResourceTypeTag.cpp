// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceTypeTag.h"

namespace ResourceTypeTag
{
	UE_DEFINE_GAMEPLAY_TAG(None, "None");

	UE_DEFINE_GAMEPLAY_TAG(Bread, "ResourceTypeTag.Bread");
	UE_DEFINE_GAMEPLAY_TAG(Wheat, "ResourceTypeTag.Wheat");
	UE_DEFINE_GAMEPLAY_TAG(WaterMelon, "ResourceTypeTag.WaterMelon");
	UE_DEFINE_GAMEPLAY_TAG(SunFlower, "ResourceTypeTag.SunFlower");
	UE_DEFINE_GAMEPLAY_TAG(Money, "ResourceTypeTag.Money");

	UE_DEFINE_GAMEPLAY_TAG(Water, "CropResourceType.Water");
	UE_DEFINE_GAMEPLAY_TAG(Light, "CropResourceType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Love, "CropResourceType.Love");
	UE_DEFINE_GAMEPLAY_TAG(Damage, "CropResourceType.Damage");
};
