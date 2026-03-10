// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "PerkManager.h"

// UE
#include "Kismet/GameplayStatics.h"

UPerkManager* FarmFPSUtilities::GetPlayerPerkManager(const UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (ensure(IsValid(playerController)) && ensure(IsValid(playerController->GetPawn())))
	{
		return playerController->GetPawn()->FindComponentByClass<UPerkManager>();
	}
	return nullptr;
}

float FarmFPSUtilities::GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, EPerkModifiers perkType, float valueToModify)
{
	UPerkManager* perkManager = GetPlayerPerkManager(WorldContextObject);
	if (ensure(IsValid(perkManager)))
	{
		return perkManager->ModifyValueByPerks(perkType, valueToModify);
	}

	return 0.f;
}
