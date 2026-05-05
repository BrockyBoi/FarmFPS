// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "ActorLookupComponent.h"
#include "ActorPool.h"
#include "BreadStand.h"
#include "CustomerSpawnerManager.h"
#include "DayNightCycleManager.h"
#include "ObjectiveManager.h"
#include "PerkManager.h"

// UE
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AGameStateBase* FarmFPSUtilities::GetGameBaseState(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)))
	{
		return nullptr;
	}

	AGameStateBase* gameState = WorldContextObject->GetWorld()->GetGameState();
	if (ensure(IsValid(gameState)))
	{
		return gameState;
	}

	return nullptr;
}

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
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UObjectiveManager* objectiveManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UObjectiveManager>();
	if (ensure(IsValid(objectiveManager)))
	{
		return objectiveManager;
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
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UCustomerSpawnerManager* customerSpawnerManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UCustomerSpawnerManager>();
	if (ensure(IsValid(customerSpawnerManager)))
	{
		return customerSpawnerManager;
	}

	return nullptr;
}

UDayNightCycleManager* FarmFPSUtilities::GetDayNightCycleManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UDayNightCycleManager* objectiveManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UDayNightCycleManager>();
	if (ensure(IsValid(objectiveManager)))
	{
		return objectiveManager;
	}

	return nullptr;
}

UActorLookupComponent* FarmFPSUtilities::GetResourceActorLookupComponent(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UActorLookupComponent* lookupComponent = GetGameBaseState(WorldContextObject)->FindComponentByClass<UActorLookupComponent>();
	if (ensure(IsValid(lookupComponent)))
	{
		return lookupComponent;
	}

	return nullptr;
}

UActorPool* FarmFPSUtilities::GetActorPool(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UActorPool* actorPool = GetGameBaseState(WorldContextObject)->FindComponentByClass<UActorPool>();
	if (ensure(IsValid(actorPool)))
	{
		return actorPool;
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
