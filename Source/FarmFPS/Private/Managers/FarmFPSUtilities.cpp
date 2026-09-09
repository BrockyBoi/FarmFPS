// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "CustomerSpawnerManager.h"
#include "Interactables/BreadStand.h"
#include "Managers/ActorLookupComponent.h"
#include "Managers/ActorPool.h"
#include "Managers/BreadRequirementManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/ObjectiveManager.h"
#include "Managers/PerkManager.h"
#include "Managers/TradeOffUpgradeManager.h"
#include "Managers/Weather/WeatherManager.h"
#include "Managers/TutorialScreenManager.h"
#include "StatusEffects/EffectManager.h"
#include "SaveSystem/SaveGameManager.h"

// UE
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AActor* UFarmFPSUtilities::GetPlayerCharacter(const UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (ensure(IsValid(playerController)) && ensure(IsValid(playerController->GetPawn())))
	{
		return playerController->GetPawn();
	}

	return nullptr;
}

AGameStateBase* UFarmFPSUtilities::GetGameBaseState(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)))
	{
		return nullptr;
	}

	AGameStateBase* gameState = WorldContextObject->GetWorld()->GetGameState();
	if (IsValid(gameState))
	{
		return gameState;
	}

	return nullptr;
}

UPerkManager* UFarmFPSUtilities::GetPlayerPerkManager(const UObject* WorldContextObject)
{
	if (ensure(IsValid(GetPlayerCharacter(WorldContextObject))))
	{
		return GetPlayerCharacter(WorldContextObject)->FindComponentByClass<UPerkManager>();
	}

	return nullptr;
}

UObjectiveManager* UFarmFPSUtilities::GetObjectiveManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UObjectiveManager* objectiveManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UObjectiveManager>();
	if (IsValid(objectiveManager))
	{
		return objectiveManager;
	}

	return nullptr;
}

ABreadStand* UFarmFPSUtilities::GetBreadStand(const UObject* WorldContextObject)
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

UCustomerSpawnerManager* UFarmFPSUtilities::GetCustomerSpawnerManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UCustomerSpawnerManager* customerSpawnerManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UCustomerSpawnerManager>();
	if (IsValid(customerSpawnerManager))
	{
		return customerSpawnerManager;
	}

	return nullptr;
}

UDayNightCycleManager* UFarmFPSUtilities::GetDayNightCycleManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UDayNightCycleManager* dayNightCycleManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UDayNightCycleManager>();
	if (IsValid(dayNightCycleManager))
	{
		return dayNightCycleManager;
	}

	return nullptr;
}

UActorLookupComponent* UFarmFPSUtilities::GetResourceActorLookupComponent(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UActorLookupComponent* lookupComponent = GetGameBaseState(WorldContextObject)->FindComponentByClass<UActorLookupComponent>();
	if (IsValid(lookupComponent))
	{
		return lookupComponent;
	}

	return nullptr;
}

UActorPool* UFarmFPSUtilities::GetActorPool(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UActorPool* actorPool = GetGameBaseState(WorldContextObject)->FindComponentByClass<UActorPool>();
	if (IsValid(actorPool))
	{
		return actorPool;
	}

	return nullptr;
}

UBreadRequirementManager* UFarmFPSUtilities::GetBreadRequirementManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UBreadRequirementManager* breadRequirementManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UBreadRequirementManager>();
	if (IsValid(breadRequirementManager))
	{
		return breadRequirementManager;
	}

	return nullptr;
}

UTradeOffUpgradeManager* UFarmFPSUtilities::GetTradeOffUpgradeManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UTradeOffUpgradeManager* tradeOffUpgradeManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UTradeOffUpgradeManager>();
	if (IsValid(tradeOffUpgradeManager))
	{
		return tradeOffUpgradeManager;
	}

	return nullptr;
}

UTutorialScreenManager* UFarmFPSUtilities::GetTutorialScreenManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UTutorialScreenManager* tutorialScreenManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UTutorialScreenManager>();
	if (IsValid(tutorialScreenManager))
	{
		return tutorialScreenManager;
	}

	return nullptr;
}

UEffectManager* UFarmFPSUtilities::GetEffectManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UEffectManager* effectManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UEffectManager>();
	if (IsValid(effectManager))
	{
		return effectManager;
	}

	return nullptr;
}

UWeatherManager* UFarmFPSUtilities::GetWeatherManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	UWeatherManager* weatherManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<UWeatherManager>();
	if (IsValid(weatherManager))
	{
		return weatherManager;
	}

	return nullptr;
}

USaveGameManager* UFarmFPSUtilities::GetSaveGameManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	USaveGameManager* saveGameManager = GetGameBaseState(WorldContextObject)->FindComponentByClass<USaveGameManager>();
	if (IsValid(saveGameManager))
	{
		return saveGameManager;
	}

	return nullptr;
}

float UFarmFPSUtilities::GetModifiedValueByPlayerPerk(const UObject* WorldContextObject, const FGameplayTag& perkTag, float valueToModify)
{
	if (perkTag == FGameplayTag::EmptyTag)
	{
		return valueToModify;
	}

	UPerkManager* perkManager = GetPlayerPerkManager(WorldContextObject);
	if (IsValid(perkManager))
	{
		return perkManager->ModifyValueByPerks(perkTag, valueToModify);
	}

	return 0.f;
}

float UFarmFPSUtilities::GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, const FGameplayTagContainer& perkTags, float valueToModify)
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
