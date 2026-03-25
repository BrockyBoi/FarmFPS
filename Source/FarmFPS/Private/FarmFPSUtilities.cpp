// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "BreadStand.h"
#include "CustomerSpawnerManager.h"
#include "ObjectiveManager.h"
#include "PerkManager.h"

// UE
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

UPerkManager* FarmFPSUtilities::GetPlayerPerkManager(const UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
;	if (ensure(IsValid(playerController)) && ensure(IsValid(playerController->GetPawn())))
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

ABreadStand* FarmFPSUtilities::GetBreadStand(const UObject* WorldContextObject)
{
	TArray<AActor*> breadStands;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ABreadStand::StaticClass(), breadStands);
	if (ensure(breadStands.Num() > 0))
	{
		ABreadStand* breadStand = Cast<ABreadStand>(breadStands[0]);
		return ensure(IsValid(breadStand)) ? breadStand : nullptr;
	}
	return nullptr;
}

UCustomerSpawnerManager* FarmFPSUtilities::GetCustomerSpawnerManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)))
	{
		return nullptr;
	}

	AGameStateBase* gameState = WorldContextObject->GetWorld()->GetGameState();
	if (ensure(IsValid(gameState)))
	{
		UCustomerSpawnerManager* customerSpawnerManager = gameState->FindComponentByClass<UCustomerSpawnerManager>();
		if (ensure(IsValid(customerSpawnerManager)))
		{
			return customerSpawnerManager;
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
