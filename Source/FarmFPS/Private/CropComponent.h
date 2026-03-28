// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "CropData.h"

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

	bool IsCropReadyToBreak() const;

	DECLARE_MULTICAST_DELEGATE(FOnCropBreakEvent);

	FOnCropBreakEvent OnCropBreak;

	void BreakCrop();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void AffectGrowth();

	UFUNCTION()
	void OnBreakCropTimerEnd();

	UPROPERTY(EditDefaultsOnly)
	FCropData _cropData;

	UResourceInventory* _cropResourcesInventory = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	TSubclassOf<AResourcePickupActor> _cropYieldPickupClass;

	UPROPERTY(EditDefaultsOnly)
	float _yieldPickupSpawnHeight = 100.f;

	UPROPERTY(EditDefaultsOnly)
	bool _breakCropOnFull = false;

	bool _isBroken = false;
};
