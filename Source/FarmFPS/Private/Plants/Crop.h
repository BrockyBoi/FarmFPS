// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Plants/CropData.h"
#include "Managers/ModifiedValueData.h"
#include "Plants/Plant.h"
#include "Managers/PoolableActor.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "Crop.generated.h"

class AResourcePickupActor;
class UCapsuleComponent;
class UResourceInventory;

UCLASS()
class ACrop : public APlant, public IPoolableActor
{
	GENERATED_BODY()

public:
	ACrop();

	void AddActorToPool();
	void RemoveFromPool();

	UFUNCTION(BlueprintPure)
	bool GetIsInPerfectTiming() const { return _isInPerfectTiming; }

	UCapsuleComponent* GetCapsuleComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnAddedToPool();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnRemovedFromPool();

	void OnPerfectTimingEnd();

	void ShowPerfectTimingVisuals();

	virtual void OnLightAndWaterFilled() override;

	UFUNCTION(BlueprintImplementableEvent)
	void Cosmetic_OnShouldShowCropHealth(bool showCropHealth);

	UFUNCTION()
	void OnBreakCropTimerEnd();

	virtual void OnPlayerDestroyPlant() override;
	void SpawnResourceActors();
	virtual void DestroyPlant() override;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	FModifiedFloatValue _perfectTimingYieldBonus = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	FModifiedFloatValue _perfectTimingDuration = 5.f;

	UPROPERTY(EditDefaultsOnly)
	float _yieldPickupSpawnHeight = 100.f;

	bool _hasStartedPerfectTiming = false;
	bool _isInPerfectTiming = false;
	bool _hasPerfectTimingPeriodEnded = false;

	UPROPERTY(EditDefaultsOnly)
	float _maxSizeModifierForPerfectTiming = 1.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onBreakCropSound;

	float _sinAngleInPerfectTiming = 270.f;

	int _currentCropHealth = 0;

	FTimerHandle _perfectTimingTimerHandle;
};
