// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

namespace ObjectiveTypeTags
{
	static const FGameplayTag CraftResource = FGameplayTag::RequestGameplayTag(FName("ObjectiveType.CraftResource"));
	static const FGameplayTag CollectResource = FGameplayTag::RequestGameplayTag(FName("ObjectiveType.CollectResource"));
	static const FGameplayTag FinishCrop = FGameplayTag::RequestGameplayTag(FName("ObjectiveType.FinishCrop"));
	static const FGameplayTag PlantCrop = FGameplayTag::RequestGameplayTag(FName("ObjectiveType.PlantCrop"));
	static const FGameplayTag SellBread = FGameplayTag::RequestGameplayTag(FName("ObjectiveType.SellBread"));
};
