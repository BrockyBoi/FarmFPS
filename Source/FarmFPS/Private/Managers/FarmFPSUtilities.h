// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "FarmFPSUtilities.generated.h"

class ABreadStand;
class AGameStateBase;
class UActorLookupComponent;
class UActorPool;
class UBreadRequirementManager;
class UCustomerSpawnerManager;
class UDayNightCycleManager;
class UEffectManager;
class UPerkManager;
class UObjectiveManager;
class UTradeOffUpgradeManager;
class UTutorialScreenManager;
class UWeatherManager;

UCLASS()
class UFarmFPSUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static AActor* GetPlayerCharacter(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static AGameStateBase* GetGameBaseState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UPerkManager* GetPlayerPerkManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UObjectiveManager* GetObjectiveManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))		
	static ABreadStand* GetBreadStand(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UCustomerSpawnerManager* GetCustomerSpawnerManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UDayNightCycleManager* GetDayNightCycleManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UActorLookupComponent* GetResourceActorLookupComponent(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UActorPool* GetActorPool(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UBreadRequirementManager* GetBreadRequirementManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UTradeOffUpgradeManager* GetTradeOffUpgradeManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UTutorialScreenManager* GetTutorialScreenManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UEffectManager* GetEffectManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UWeatherManager* GetWeatherManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static float GetModifiedValueByPlayerPerks(const UObject* WorldContextObject, const FGameplayTagContainer& perkTags, float valueToModify);

	UFUNCTION(BlueprintPure, Category = "FarmFPS|Utilities", meta = (WorldContext = "WorldContextObject"))
	static float GetModifiedValueByPlayerPerk(const UObject* WorldContextObject, const FGameplayTag& perkTag, float valueToModify);
};
