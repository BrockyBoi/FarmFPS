// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "Managers/ActorLookupComponent.h"
#include "Managers/ActorPool.h"
#include "Managers/BreadRequirementManager.h"
#include "Interactables/BreadStand.h"
#include "CustomerSpawnerManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/ObjectiveManager.h"
#include "Managers/PerkManager.h"
#include "Managers/TradeOffUpgradeManager.h"
#include "Managers/WeatherManager.h"
#include "StatusEffects/EffectManager.h"

// UE
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AActor* FarmFPSUtilities::GetPlayerCharacter(const UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (ensure(IsValid(playerController)) && ensure(IsValid(playerController->GetPawn())))
	{
		return playerController->GetPawn();
	}

	return nullptr;
}

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
	if (ensure(IsValid(GetPlayerCharacter(WorldContextObject))))
	{
		return GetPlayerCharacter(WorldContextObject)->FindComponentByClass<UPerkManager>();
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

UBreadRequirementManager* FarmFPSUtilities::GetBreadRequirementManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UBreadRequirementManager* breadRequirementManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UBreadRequirementManager>();
	if (ensure(IsValid(breadRequirementManager)))
	{
		return breadRequirementManager;
	}

	return nullptr;
}

UTradeOffUpgradeManager* FarmFPSUtilities::GetTradeOffUpgradeManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UTradeOffUpgradeManager* tradeOffUpgradeManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UTradeOffUpgradeManager>();
	if (ensure(IsValid(tradeOffUpgradeManager)))
	{
		return tradeOffUpgradeManager;
	}

	return nullptr;
}

UEffectManager* FarmFPSUtilities::GetEffectManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UEffectManager* effectManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UEffectManager>();
	if (ensure(IsValid(effectManager)))
	{
		return effectManager;
	}

	return nullptr;
}

UWeatherManager* FarmFPSUtilities::GetWeatherManager(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(GetGameBaseState(WorldContextObject))))
	{
		return nullptr;
	}

	UWeatherManager* weatherManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UWeatherManager>();
	if (ensure(IsValid(weatherManager)))
	{
		return weatherManager;
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
