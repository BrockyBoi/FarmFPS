// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock6
#include "Resources/ResourceConverterStationActor.h"

// UE 
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

// Generated
#include "BreadOven.generated.h"

UCLASS()
class ABreadOven : public AResourceConverterStationActor
{
	GENERATED_BODY()
	
public:
	ABreadOven();

	UFUNCTION(BlueprintPure)
	float GetHeatMultipler() const;
	
	UFUNCTION(BlueprintPure)
	float GetOvenHeat() const { return _ovenHeat; }

	UFUNCTION(BlueprintPure)
	bool ShouldGainHeatMultiplier() const;

	UFUNCTION(BlueprintPure)
	bool IsHeatTooHigh() const;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnOvenHeatChanged, float);
	FOnOvenHeatChanged OnOvenHeatChanged;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnFireTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual float GetTimeBetweenSpawns() const override;
	virtual void SpawnResource(ResourcesToSpawnData& data) override;

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnOvenHeatChanged(float newHeat);

	UFUNCTION(BlueprintPure)
	FLinearColor GetHeatColorText() const;

	void SetHeatLevel(float newHeat);

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _idealHeatMin = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _idealHeatMax = 65.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _heatGainOnLight = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _heatLossOnWater = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _heatLostPerSecond = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _heatToDestroyBread = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FModifiedFloatValue _coldHeatSpeedPenalty = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FModifiedFloatValue _idealHeatSpeedModifier = .5f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FModifiedFloatValue _heatDissipationPerSecond = .1f;

	UPROPERTY(BlueprintReadOnly)
	float _ovenHeat = 0.f;

	UPROPERTY(EditAnywhere)
	USphereComponent* _fireTargetCollider = nullptr;
};
