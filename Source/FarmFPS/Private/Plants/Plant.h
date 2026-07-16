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

UCLASS()
class APlant : public AActor
{
	GENERATED_BODY()
	
public:	
	APlant();

	UFUNCTION(BlueprintCallable)
	void AddResource(const FGameplayTag& resourceType, float amount);
	const FCropData& GetCropData() const { return _cropData; }

	void DoDamage(int damageAmount);

	UResourceInventory* GetResourceInventory() const { return _resourcesInventory; }

	UFUNCTION(BlueprintPure)
	int GetCurrentWaterLevel() const;

	UFUNCTION(BlueprintPure)
	int GetCurrentLightLevel() const;

	UFUNCTION(BlueprintPure)
	float GetWaterLeft() const { return _cropData.WaterNeeded - GetCurrentWaterLevel(); }

	UFUNCTION(BlueprintPure)
	float GetWaterPercentage() const { return GetCurrentWaterLevel() / (float)_cropData.WaterNeeded; }

	UFUNCTION(BlueprintPure)
	float GetLightPercentage() const { return GetCurrentLightLevel() / (float)_cropData.LightNeeded; }

	UFUNCTION(BlueprintPure)
	float GetLightLeft() const { return _cropData.LightNeeded - GetCurrentLightLevel(); }

	UFUNCTION(BlueprintPure)
	float GetCompletionPercentage() const;

	UFUNCTION(BlueprintPure)
	bool IsLightAndWaterFull() const;

	DECLARE_MULTICAST_DELEGATE(FOnPlantBreakEvent);
	FOnPlantBreakEvent OnPlantBreak;

protected:
	virtual void BeginPlay() override;

	virtual void DestroyPlant();

	virtual void OnLightAndWaterFilled();
	virtual void AffectGrowth();

	virtual void OnPlayerDestroyPlant();

	void InitializeInventory();

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

	UResourceInventory* _resourcesInventory = nullptr;

	int _currentPlantHealth = 0;

	UPROPERTY(EditDefaultsOnly)
	FCropData _cropData;
};
