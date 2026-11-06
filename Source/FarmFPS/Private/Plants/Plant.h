// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Data/CropGrowthData.h"
#include "Resources/ResourceInventory.h"
#include "SaveSystem/Saveable.h"
#include "SaveSystem/PlantSaveData.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "Plant.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFullyGrown)

class UWeatherManager;

UCLASS()
class APlant : public AActor, public ISaveable
{
	GENERATED_BODY()
	
public:	
	APlant();

	UFUNCTION(BlueprintCallable)
	virtual void AddResource(const FGameplayTag& resourceType, float amount);
	void AddResource(const FGameplayTagContainer& resourceTypes, float amount);
	const FPlantGrowthData& GetPlantGrowthData() const { return _plantGrowthData; }

	void DoDamage(int damageAmount);
	void SetIsInvincible(bool isInvincible);

	virtual void OnGameLoaded(UFarmFPSSaveGame* saveGame) override;

	UResourceInventory* GetResourceInventory() const { return _resourcesInventory; }
	FPlantSaveData GetPlantSaveData() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentWaterLevel() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentLightLevel() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentLoveLevel() const;

	UFUNCTION(BlueprintPure)
	float GetWaterLeft() const { return _plantGrowthData.WaterNeeded.GetModifiedValue(this) - GetCurrentWaterLevel(); }

	UFUNCTION(BlueprintPure)
	float GetWaterPercentage() const { return GetCurrentWaterLevel() / (float)_plantGrowthData.WaterNeeded.GetModifiedValue(this); }

	UFUNCTION(BlueprintPure)
	float GetLightPercentage() const { return GetCurrentLightLevel() / (float)_plantGrowthData.LightNeeded.GetModifiedValue(this); }

	UFUNCTION(BlueprintPure)
	float GetLightLeft() const { return _plantGrowthData.LightNeeded.GetModifiedValue(this) - GetCurrentLightLevel(); }

	UFUNCTION(BlueprintPure)
	bool IsLightFull() const { return FMath::IsNearlyEqual(1.f, GetLightPercentage()); }

	UFUNCTION(BlueprintPure)
	bool IsWaterFull() const { return FMath::IsNearlyEqual(1.f, GetWaterPercentage()); }

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

	static FOnFullyGrown OnFullyGrown;

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

	void SetNewGrowthPercentage(float growthPercentage, bool instant);

	UFUNCTION()
	virtual void OnStormBegin();

	UFUNCTION()
	virtual void OnStormEnd();

	virtual void OnDayStateChange(EDayState newState);

	bool _isBroken = false;
	bool _isLightAndWaterFull = false;

	UPROPERTY(EditAnywhere)
	bool _breakCropOnFull = false;

	UPROPERTY(EditAnywhere)
	bool _destroyAtEndOfDay = false;

	UPROPERTY(EditAnywhere)
	bool _isInvincible = false;

	float _adjustedGrowthPercentage = 0.f;
	float _previousScalePercentage = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float _lerpSpeedScale = .01f;

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnResourceAdded();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnResourceFull(const FGameplayTag& resourceType);

	UResourceInventory* _resourcesInventory = nullptr;
	UStaticMeshComponent* _staticMesh = nullptr;

	int _currentPlantHealth = 0;

	FVector _startingScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant|Growth")
	FPlantGrowthData _plantGrowthData;

	TWeakObjectPtr<UWeatherManager> _weatherManager = nullptr;
};
