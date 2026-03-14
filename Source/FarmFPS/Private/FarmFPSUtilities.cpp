// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "ObjectiveManager.h"
#include "PerkManager.h"

// UE
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"

UPerkManager* FarmFPSUtilities::GetPlayerPerkManager(const UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (ensure(IsValid(playerController)) && ensure(IsValid(playerController->GetPawn())))
	{
		return playerController->GetPawn()->FindComponentByClass<UPerkManager>();
	}
	return nullptr;
}

UObjectiveManager* FarmFPSUtilities::GetObjectiveManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)))
	{
		return nullptr;
	}

	AGameStateBase* gameState = WorldContextObject->GetWorld()->GetGameState();
	if (ensure(IsValid(gameState)))
	{
		UObjectiveManager* objectiveManager = gameState->FindComponentByClass<UObjectiveManager>();
		if (ensure(IsValid(objectiveManager)))
		{
			return objectiveManager;
		}
	}

	return nullptr;
}

float FarmFPSUtilities::GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, const FGameplayTag& perkTag, float valueToModify)
{
	if (perkTag == FGameplayTag::EmptyTag)
	{
		return valueToModify;
	}

	UPerkManager* perkManager = GetPlayerPerkManager(WorldContextObject);
	if (ensure(IsValid(perkManager)))
	{
		return perkManager->ModifyValueByPerks(perkTag, valueToModify);
	}

	return 0.f;
}

float FarmFPSUtilities::GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, const FGameplayTagContainer& perkTags, float valueToModify)
{
	if (perkTags.IsEmpty())
	{
		return valueToModify;
	}

	UPerkManager* perkManager = GetPlayerPerkManager(WorldContextObject);
	if (ensure(IsValid(perkManager)))
	{
		return perkManager->ModifyValueByPerks(perkTags, valueToModify);
	}

	return 0.f;
}
