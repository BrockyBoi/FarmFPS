// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class ABreadStand;
class AGameStateBase;
class UActorPool;
class UCustomerSpawnerManager;
class UDayNightCycleManager;
class UActorLookupComponent;
class UPerkManager;
class UObjectiveManager;

class FarmFPSUtilities
{
public:
	static AGameStateBase* GetGameBaseState(const UObject* WorldContextObject);
	static UPerkManager* GetPlayerPerkManager(const UObject* WorldContextObject);
	static UObjectiveManager* GetObjectiveManager(const UObject* WorldContextObject);
	static ABreadStand* GetBreadStand(const UObject* WorldContextObject);
	static UCustomerSpawnerManager* GetCustomerSpawnerManager(const UObject* WorldContextObject);
	static UDayNightCycleManager* GetDayNightCycleManager(const UObject* WorldContextObject);
	static UActorLookupComponent* GetResourceActorLookupComponent(const UObject* WorldContextObject);
	static UActorPool* GetActorPool(const UObject* WorldContextObject);
	static float GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, const FGameplayTagContainer& perkTags, float valueToModify);
	static float GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, const FGameplayTag& perkTag, float valueToModify);
};
