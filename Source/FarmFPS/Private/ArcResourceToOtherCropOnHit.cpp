// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcResourceToOtherCropOnHit.h"

// Brock
#include "Plants/Crop.h"

void UArcResourceToOtherCropOnHit::OnEffectStarted()
{
	_timesArced = 0;
	if (ensure(_cropToAffect.IsValid()))
	{
		TArray<FHitResult> hitResults;
		FCollisionShape sphere = FCollisionShape::MakeSphere(_radiusToCheck.GetModifiedValue(this));
		FCollisionQueryParams params;
		params.AddIgnoredActor(_cropToAffect.Get());
		GetWorld()->SweepMultiByChannel(hitResults, _cropToAffect->GetActorLocation(), _cropToAffect->GetActorLocation(), FQuat::Identity, _collisionChannel, sphere);

		for (FHitResult& result : hitResults)
		{
			ACrop* crop = Cast<ACrop>(result.GetActor());
			if (IsValid(crop) && crop->GetCropData().ResourceType == _resourceType)
			{
				ArcToCrop(crop);

				_timesArced++;
				if (_timesArced >= _maxArcCount.GetModifiedValue(this))
				{
					StopEffect();
					break;
				}
			}
		}
	}
}

void UArcResourceToOtherCropOnHit::ArcToCrop(ACrop* otherCrop)
{
	if (ensure(IsValid(otherCrop)))
	{
		otherCrop->AddResource(_resourceType, _baseResourceAmount * _percentageResourceReceivedOnArc.GetModifiedValue(this));
	}
}
