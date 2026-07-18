// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "OnCropHitStatusEffect.h"

// UE
#include "CoreMinimal.h"

/**
 * 
 */
class ArcResourceToOtherCropOnHit : public OnCropHitStatusEffect
{
public:
	ArcResourceToOtherCropOnHit(FGameplayTag resourceType, APlant* plant, float baseResourceAmount, int maxArcCount, float resourcePercentageToGive, ECollisionChannel cropCollisionChannel);
	void SetBaseResourceAmount(float resourceAmount) { _baseResourceAmount = resourceAmount; }
	virtual void StartEffect() override;

protected:

	int _timesArced = 0;

	float _baseResourceAmount = 0.f;

	int _maxArcCount = 1;
	float _resourcePercentageToGive = .33f;
	float _radiusToCheck = 100.f;

	ECollisionChannel _collisionChannel;

private:
	void ArcToCrop(ACrop* otherCrop);
};
