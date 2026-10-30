// Fill out your copyright notice in the Description page of Project Settings.

#include "FarmFPSUtilities.h"

// Brock
#include "CustomerSpawnerManager.h"
#include "Interactables/BreadStand.h"
#include "Managers/ActorLookupComponent.h"
#include "Managers/ActorPool.h"
#include "Managers/AudioManager.h"
#include "Managers/BreadRequirementManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/ObjectiveManager.h"
#include "Managers/PerkManager.h"
#include "Managers/TradeOffUpgradeManager.h"
#include "Managers/TutorialScreenManager.h"
#include "Managers/VisualSettingsManager.h"
#include "Managers/Weather/WeatherManager.h"
#include "ModifiedValueData.h"
#include "StatusEffects/EffectManager.h"
#include "SaveSystem/SaveGameManager.h"

// UE
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AActor* UFarmFPSUtilities::GetPlayerCharacter(const UObject* WorldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (IsValid(playerController) && ensure(IsValid(playerController->GetPawn())))
	{
		return playerController->GetPawn();
	}

	return nullptr;
}

AGameStateBase* UFarmFPSUtilities::GetGameBaseState(const UObject* WorldContextObject)
{
	if (!ensure(IsValid(WorldContextObject)) || !ensure(IsValid(WorldContextObject->GetWorld())))
	{
		return nullptr;
	}

	return WorldContextObject->GetWorld()->GetGameState();
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

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UObjectiveManager>();
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

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UCustomerSpawnerManager>();
}

UDayNightCycleManager* UFarmFPSUtilities::GetDayNightCycleManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UDayNightCycleManager>();
}

UActorLookupComponent* UFarmFPSUtilities::GetResourceActorLookupComponent(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UActorLookupComponent>();
}

UActorPool* UFarmFPSUtilities::GetActorPool(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UActorPool>();
}

UBreadRequirementManager* UFarmFPSUtilities::GetBreadRequirementManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UBreadRequirementManager>();
}

UTradeOffUpgradeManager* UFarmFPSUtilities::GetTradeOffUpgradeManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UTradeOffUpgradeManager>();
}

UTutorialScreenManager* UFarmFPSUtilities::GetTutorialScreenManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UTutorialScreenManager>();
}

UEffectManager* UFarmFPSUtilities::GetEffectManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UEffectManager>();
}

UWeatherManager* UFarmFPSUtilities::GetWeatherManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UWeatherManager>();
}

USaveGameManager* UFarmFPSUtilities::GetSaveGameManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<USaveGameManager>();
}

UAudioManager* UFarmFPSUtilities::GetAudioManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UAudioManager>();
}

UVisualSettingsManager* UFarmFPSUtilities::GetVisualSettingsManager(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject) || !IsValid(GetGameBaseState(WorldContextObject)))
	{
		return nullptr;
	}

	return GetGameBaseState(WorldContextObject)->FindComponentByClass<UVisualSettingsManager>();
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
