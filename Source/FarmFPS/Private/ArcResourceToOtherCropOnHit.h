// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "OnCropHitEffect.h"

// UE
#include "CoreMinimal.h"

// UE
#include "ArcResourceToOtherCropOnHit.generated.h"

/**
 * 
 */
UCLASS()
class UArcResourceToOtherCropOnHit : public UOnCropHitEffect
{
	GENERATED_BODY()

public:
	void SetBaseResourceAmount(float resourceAmount) { _baseResourceAmount = resourceAmount; }

protected:
	virtual void OnEffectStarted() override;

	int _timesArced = 0;

	float _baseResourceAmount = 0.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedIntValue _maxArcCount = 1;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _percentageResourceReceivedOnArc = .33f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _radiusToCheck = 100.f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> _collisionChannel = ECollisionChannel::ECC_Visibility;

private:
	void ArcToCrop(ACrop* otherCrop);
};
