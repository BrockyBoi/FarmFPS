// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ResourceConverterStationActor.h"

// UE 
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"

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

protected:
	UFUNCTION()
	void OnFireTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual float GetTimeBetweenSpawns() const override;
	virtual void SpawnResource(ResourcesToSpawnData& data) override;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _idealHeatMin = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _idealHeatMax = 65.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	float _heatToDestroyBread = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FModifiedFloatValue _idealHeatSpeedModifier = .5f;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FModifiedFloatValue _heatDissipationPerSecond = .1f;

	float _ovenHeat = 0.f;

	UPROPERTY(EditAnywhere)
	USphereComponent* _fireTargetCollider = nullptr;
};
