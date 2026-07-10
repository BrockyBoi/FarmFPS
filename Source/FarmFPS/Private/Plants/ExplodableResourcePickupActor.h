// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Resources/GrenadeActor.h"

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
class AExplodableResourcePickupActor : public AGrenadeActor
{
	GENERATED_BODY()

protected:
	virtual void ExplosionAffectActor(AActor* actor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedFloatValue _explosionResourceValue = 10.f;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "CropResourceType,ResourceType"))
	FGameplayTag _explosionResourceType;
};
