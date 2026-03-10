// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"

class UPerkManager;

class FarmFPSUtilities
{
public:
	static UPerkManager* GetPlayerPerkManager(const UObject* WorldContextObject);
	static float GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, EPerkModifiers perkType, float valueToModify);
};
