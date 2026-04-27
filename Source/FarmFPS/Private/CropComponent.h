// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "CropData.h"
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "CropComponent.generated.h"

class AResourcePickupActor;
class UResourceInventory;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCropComponent();

	UFUNCTION(BlueprintCallable)
	void AddCropResourceValue(const FGameplayTag& resourceType, float amount);

	void DoDamageToCrop(int damageAmount);

	UResourceInventory* GetResourceInventory() const { return _cropResourcesInventory; }

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
	float GetCropCompletionPercentage() const;

	UFUNCTION(BlueprintPure)
	bool GetIsInPerfectTiming() const { return _isInPerfectTiming; }

	bool IsLightAndWaterFull() const;

	DECLARE_MULTICAST_DELEGATE(FOnCropBreakEvent);

	FOnCropBreakEvent OnCropBreak;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

	void OnLightAndWaterFilled();
	void OnPerfectTimingEnd();

	void AffectGrowth();

	UFUNCTION()
	void OnDayEnd();

	UFUNCTION()
	void OnBreakCropTimerEnd();

	void BreakCrop();
	void DestroyCrop();

	UPROPERTY(EditDefaultsOnly)
	FCropData _cropData;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	FModifiedFloatValue _perfectTimingYieldBonus = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	FModifiedFloatValue _perfectTimingDuration = 5.f;

	UResourceInventory* _cropResourcesInventory = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	TSubclassOf<AResourcePickupActor> _cropYieldPickupClass;

	UPROPERTY(EditDefaultsOnly)
	float _yieldPickupSpawnHeight = 100.f;

	UPROPERTY(EditDefaultsOnly)
	bool _breakCropOnFull = false;

	bool _isBroken = false;

	bool _hasStartedPerfectTiming = false;
	bool _isInPerfectTiming = false;

	UPROPERTY(EditDefaultsOnly)
	float _maxSizeModifierForPerfectTiming = 1.15f;

	float _sinAngleInPerfectTiming = 270.f;

	int _currentCropHealth = 0;

	FTimerHandle _perfectTimingTimerHandle;
};
