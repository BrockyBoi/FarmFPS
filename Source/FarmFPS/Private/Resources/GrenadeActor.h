// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "Resources/ResourcePickupActor.h"

// UE
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"

// Generated
#include "GrenadeActor.generated.h"

/**
 * 
 */
UCLASS()
class AGrenadeActor : public AResourcePickupActor
{
	GENERATED_BODY()
	
public:
	AGrenadeActor();

	virtual void AddActorToPool() override;
	virtual void RemoveFromPool() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnThrownOnGround() override;

	virtual void ExplosionAffectActor(AActor* actor);

	UFUNCTION()
	void OnExplosionTimerFinished();

	FTimerHandle _explosionTimerHandle;

	UPROPERTY(EditAnywhere)
	USphereComponent* _explosionCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedFloatValue _explosionRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedFloatValue _explosionForce = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool _appliesPhysicalForce = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float _explosionDuration = 0.5f;
};
