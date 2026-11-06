// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Data/CropGrowthData.h"
#include "Data/CropDataAsset.h"
#include "Data/ModifiedValueData.h"
#include "Interactables/MeleeableInterface.h"
#include "Managers/PoolableActor.h"
#include "Plants/Plant.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "Crop.generated.h"

class UCapsuleComponent;
class UDynamicMaterialInstance;
class UResourceInventory;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCropPlanted, ACrop*);

UCLASS()
class ACrop : public APlant, public IPoolableActor, public IMeleeableInterface
{
	GENERATED_BODY()

public:
	ACrop();

	void AddActorToPool();
	void RemoveFromPool();

	void SetWasPlantedSuccessfully(bool wasSuccessful);

	virtual void AddResource(const FGameplayTag& resourceType, float amount) override;

	virtual void MeleeObject(int damage) override;

	UFUNCTION(BlueprintPure)
	bool GetIsInPerfectTiming() const { return _isInPerfectTiming; }

	UCapsuleComponent* GetCapsuleComponent() const;

	const UCropDataAsset* GetCropDataAsset() const;

	static FOnCropPlanted OnCropPlanted;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

	void SetMeshForCurrentProgress();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnAddedToPool();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnRemovedFromPool();

	void OnPerfectTimingEnd();

	void ShowCropCompletionVisuals();

	virtual void OnLightAndWaterFilled() override;

	UFUNCTION(BlueprintImplementableEvent)
	void Cosmetic_OnShouldShowCropHealth(bool showCropHealth);

	UFUNCTION()
	void OnBreakCropTimerEnd();

	virtual void OnPlayerDestroyPlant() override;
	void SpawnResourceActors();
	virtual void DestroyPlant() override;

	virtual bool ShouldTick() const override;

	void AffectCropColor();

	UFUNCTION()
	void SpawnNiagaraComponent(UObject* niagaraObject, const FGameplayTag& resourceTag);

	void EnableStartingNiagaraSystems();

	UPROPERTY(Transient)
	UNiagaraComponent* _waterVFX = nullptr;

	UPROPERTY(Transient)
	UNiagaraComponent* _sunVFX = nullptr;

	UPROPERTY(Transient)
	UNiagaraComponent* _loveVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Crop")
	UCropDataAsset* _cropDataAsset = nullptr;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* _dynamicMaterialInstance = nullptr;

	UPROPERTY(Transient)
	UParticleSystemComponent* _onBreakVFX = nullptr;

	UPROPERTY(EditAnywhere)
	float _flashMaterialDuration = .2f;

	float _previousColorPercentage = 0;
	float _adjustedColorPercentage = 0;
	FLinearColor _previousColor = FLinearColor::Black;
	FLinearColor _adjustedColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly, Category = "Crop")
	FName _deadFoliageParameterName = TEXT("DeadFoliageColor");

	UPROPERTY(EditDefaultsOnly, Category = "Crop|Crop Yield")
	FModifiedFloatValue _perfectTimingYieldBonus = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Crop|Crop Yield")
	FModifiedFloatValue _perfectTimingDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Crop|Crop Yield")
	float _yieldPickupSpawnHeight = 100.f;

	bool _hasStartedPerfectTiming = false;
	bool _isInPerfectTiming = false;
	bool _hasPerfectTimingPeriodEnded = false;

	UPROPERTY(EditDefaultsOnly, Category = "Crop|Perfect Timing")
	float _maxSizeModifierForPerfectTiming = 1.15f;

	UPROPERTY(EditAnywhere, Category = "Crop|Perfect Timing")
	float _sinAngleInPerfectTiming = 270.f;

	UPROPERTY(EditAnywhere, Category = "Crop|Perfect Timing")
	float _changeInPerfectTimingSizePerFrame = -.75f;

	float _randomPerfectTimingModifierOffset = 0.f;

	int _currentCropHealth = 0;

	FTimerHandle _perfectTimingTimerHandle;
};
