// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "CropData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "CropComponent.generated.h"

class AResourcePickupActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCropComponent();

	UFUNCTION(BlueprintCallable)
	void AddWater(float waterAmount);

	UFUNCTION(BlueprintCallable)
	void AddLight(float lightAmount);

	UFUNCTION(BlueprintPure)
	float GetCurrentWaterLevel() const { return _currentWaterLevel; }

	UFUNCTION(BlueprintPure)
	float GetCurrentLightLevel() const { return _currentLightLevel; }

	UFUNCTION(BlueprintPure)
	float GetWaterLeft() const { return _cropData.WaterNeeded - _currentWaterLevel; }

	UFUNCTION(BlueprintPure)
	float GetWaterPercentage() const { return _currentWaterLevel / _cropData.WaterNeeded; }

	UFUNCTION(BlueprintPure)
	float GetLightPercentage() const { return _currentLightLevel / _cropData.LightNeeded; }

	UFUNCTION(BlueprintPure)
	float GetLightLeft() const { return _cropData.LightNeeded - _currentLightLevel; }

	bool IsCropReadyToBreak() const;

	void BreakCrop();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void AffectGrowth();

	UPROPERTY(EditDefaultsOnly)
	FCropData _cropData;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	TSubclassOf<AResourcePickupActor> _cropYieldPickupClass;

	UPROPERTY(EditDefaultsOnly)
	float _yieldPickupSpawnHeight = 100.f;

	UPROPERTY(EditDefaultsOnly)
	bool _breakCropOnFull = false;

	float _currentWaterLevel = 0.f;
	float _currentLightLevel = 0.f;
};
