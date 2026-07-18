// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcResourceToOtherCropOnHit.h"

// Brock
#include "Plants/Crop.h"

ArcResourceToOtherCropOnHit::ArcResourceToOtherCropOnHit(FGameplayTag resourceType, APlant* plant, float baseResourceAmount, int maxArcCount, float resourcePercentageToGive, ECollisionChannel cropCollisionChannel) : OnCropHitStatusEffect(EStatusEffectType::Arc, resourceType, plant)
{
	_baseResourceAmount = baseResourceAmount;
	_maxArcCount = maxArcCount;
	_resourcePercentageToGive = resourcePercentageToGive;
	_collisionChannel = cropCollisionChannel;
}

void ArcResourceToOtherCropOnHit::StartEffect()
{
	OnCropHitStatusEffect::StartEffect();

	_timesArced = 0;
	if (ensure(_cropToAffect.IsValid()))
	{
		TArray<FHitResult> hitResults;
		FCollisionShape sphere = FCollisionShape::MakeSphere(_radiusToCheck);
		FCollisionQueryParams params;
		params.AddIgnoredActor(_cropToAffect.Get());
		_cropToAffect->GetWorld()->SweepMultiByChannel(hitResults, _cropToAffect->GetActorLocation(), _cropToAffect->GetActorLocation(), FQuat::Identity, _collisionChannel, sphere);

		for (FHitResult& result : hitResults)
		{
			ACrop* crop = Cast<ACrop>(result.GetActor());
			if (IsValid(crop) && crop->GetCropData().ResourceType == _cropToAffect->GetCropData().ResourceType)
			{
				ArcToCrop(crop);

				_timesArced++;
				if (_timesArced >= _maxArcCount)
				{
					StopEffect();
					break;
				}
			}
		}
	}
}

void ArcResourceToOtherCropOnHit::ArcToCrop(ACrop* otherCrop)
{
	if (ensure(IsValid(otherCrop)))
	{
		otherCrop->AddResource(_resourceType, _baseResourceAmount * _resourcePercentageToGive);
	}
}
