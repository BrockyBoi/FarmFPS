// Fill out your copyright notice in the Description page of Project Settings.

#include "OnCropHitEffect.h"

// Brock
#include "Plants/Crop.h"

void UOnCropHitEffect::SetResourceType(FGameplayTag resourceType)
{
	_resourceType = resourceType;
}

void UOnCropHitEffect::TickEffect(float deltaTime)
{
	// Do stuff presumably
}

void UOnCropHitEffect::OnHitCrop(ACrop* crop)
{
	if (ensure(IsValid(crop)))
	{
		_cropToAffect = crop;

		float randomValue = FMath::RandRange(0.f, 1.f);
		if (randomValue >= _procChanceOnHit.GetModifiedValue(this))
		{
			OnEffectStarted();
		}
	}
}

void UOnCropHitEffect::OnEffectStarted()
{

}

void UOnCropHitEffect::StopEffect()
{
	_cropToAffect = nullptr;
}
