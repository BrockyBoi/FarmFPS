// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock

// UE
#include "CoreMinimal.h"

class UPerkManager;

/**
 * 
 */
class FarmFPSUtilities
{
public:
	static UPerkManager* GetPlayerPerkManager(UObject* WorldContextObject);
};
