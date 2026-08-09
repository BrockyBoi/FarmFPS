// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Plants/CropData.h"
#include "Resources/ResourceInventory.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "Plant.generated.h"

class UWeatherManager;

UCLASS()
class APlant : public AActor
{
	GENERATED_BODY()
	
public:	
	APlant();

	UFUNCTION(BlueprintCallable)
	void AddResource(const FGameplayTag& resourceType, float amount);
	void AddResource(const FGameplayTagContainer& resourceTypes, float amount);
	const FCropData& GetCropData() const { return _cropData; }

	void DoDamage(int damageAmount);

	UResourceInventory* GetResourceInventory() const { return _resourcesInventory; }

	UFUNCTION(BlueprintPure)
	float GetCurrentWaterLevel() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentLightLevel() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentLoveLevel() const;

	UFUNCTION(BlueprintPure)
	float GetWaterLeft() const { return _cropData.WaterNeeded.GetModifiedValue(this) - GetCurrentWaterLevel(); }

	UFUNCTION(BlueprintPure)
	float GetWaterPercentage() const { return GetCurrentWaterLevel() / (float)_cropData.WaterNeeded.GetModifiedValue(this); }

	UFUNCTION(BlueprintPure)
	float GetLightPercentage() const { return GetCurrentLightLevel() / (float)_cropData.LightNeeded.GetModifiedValue(this); }

	UFUNCTION(BlueprintPure)
	float GetLightLeft() const { return _cropData.LightNeeded.GetModifiedValue(this) - GetCurrentLightLevel(); }

	UFUNCTION(BlueprintPure)
	float GetCompletionPercentage() const;

	UFUNCTION(BlueprintPure)
	bool IsLightAndWaterFull() const;

	UFUNCTION(BlueprintPure)
	bool IsFullyLoved() const;

	DECLARE_MULTICAST_DELEGATE(FOnPlantBreakEvent);
	FOnPlantBreakEvent OnPlantBreak;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnResourceFullEvent, const FGameplayTag&);
	FOnResourceFullEvent OnPlantResourceFull;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void DestroyPlant();

	virtual void OnLightAndWaterFilled();
	virtual void AffectGrowth();

	virtual void OnPlayerDestroyPlant();

	void InitializeInventory();

	void ListenToWeatherManager(bool listen);

	virtual bool ShouldTick() const;

	virtual void CheckShouldTick();

	UFUNCTION()
	virtual void OnStormBegin();

	UFUNCTION()
	virtual void OnStormEnd();

	UFUNCTION()
	virtual void OnDayEnd();

	bool _isBroken = false;
	bool _isLightAndWaterFull = false;

	UPROPERTY(EditDefaultsOnly)
	bool _breakCropOnFull = false;

	UPROPERTY(EditDefaultsOnly)
	bool _destroyAtEndOfDay = false;

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnResourceAdded();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnResourceFull(const FGameplayTag& resourceType);

	UResourceInventory* _resourcesInventory = nullptr;

	int _currentPlantHealth = 0;

	UPROPERTY(EditDefaultsOnly)
	FCropData _cropData;

	TWeakObjectPtr<UWeatherManager> _weatherManager = nullptr;
};
