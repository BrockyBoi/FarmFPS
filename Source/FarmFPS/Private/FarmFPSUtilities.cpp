// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "PerkManager.h"

// UE
#include "Kismet/GameplayStatics.h"

UPerkManager* FarmFPSUtilities::GetPlayerPerkManager(UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (ensure(IsValid(playerController)) && ensure(IsValid(playerController->GetPawn())))
	{
		return playerController->GetPawn()->FindComponentByClass<UPerkManager>();
	}
	return nullptr;
}
