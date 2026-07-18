// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "OnCropHitStatusEffect.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class APlant;

/**
 * 
 */
class LingeringStatusEffectOnHit : public OnCropHitStatusEffect
{

public:
	LingeringStatusEffectOnHit(FGameplayTag resourceType, APlant* plant, float lingerDuration, float resourcesPerSecond);
	virtual void TickEffect(float deltaTime) override;
	virtual void StartEffect() override;

protected:
	virtual void StopEffect() override;

	float _currentTimeElapsed = 0.f;

	float _lingerDuration = 3.f;
	float _resourcePerSecond = 5.f;
};
