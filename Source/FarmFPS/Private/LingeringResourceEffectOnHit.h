// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "OnCropHitEffect.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "LingeringResourceEffectOnHit.generated.h"

/**
 * 
 */
UCLASS()
class ULingeringResourceEffectOnHit : public UOnCropHitEffect
{
	GENERATED_BODY()

public:
	virtual void TickEffect(float deltaTime) override;

protected:
	virtual void OnHitCrop(class ACrop* crop) override;
	virtual void OnEffectStarted() override;
	virtual void StopEffect() override;

	float _currentTimeElapsed = 0.f;

	FModifiedFloatValue _lingerDuration = 3.f;
	FModifiedFloatValue _resourcePerSecond = 5.f;
};
