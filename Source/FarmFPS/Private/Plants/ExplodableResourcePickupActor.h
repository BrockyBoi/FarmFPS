// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "Resources/ResourcePickupActor.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "ExplodableResourcePickupActor.generated.h"

class APlant;

/**
 * 
 */
UCLASS()
class AExplodableResourcePickupActor : public AResourcePickupActor
{
	GENERATED_BODY()

public:
	AExplodableResourcePickupActor();

	virtual void OnThrownOnGround() override;
	virtual void AddActorToPool() override;
	virtual void RemoveFromPool() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OverlapWithPlant(APlant* plant);

	UFUNCTION()
	void OnExplosionTimerFinished();

	FTimerHandle _explosionTimerHandle;

	UPROPERTY(EditAnywhere)
	USphereComponent* _explosionCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "CropResourceType,ResourceType"))
	FGameplayTag _explosionResourceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedFloatValue _explosionResourceValue = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedFloatValue _explosionRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float _explosionDuration = 0.5f;
};
