// Fill out your copyright notice in the Description page of Project Settings.

#include "LingeringResourceEffectOnHit.h"

// Brock
#include "Plants/Crop.h"

void ULingeringResourceEffectOnHit::TickEffect(float deltaTime)
{
	Super::TickEffect(deltaTime);

	_currentTimeElapsed += deltaTime;

	if (ensure(_cropToAffect.IsValid()))
	{
		_cropToAffect->AddResource(_resourceType, _resourcePerSecond.GetModifiedValue(this) * deltaTime);
	}

	if (_currentTimeElapsed >= _lingerDuration.GetModifiedValue(this))
	{
		StopEffect();
	}
}

void ULingeringResourceEffectOnHit::OnHitCrop(ACrop* crop)
{
	Super::OnHitCrop(crop);
}

void ULingeringResourceEffectOnHit::OnEffectStarted()
{
	Super::OnEffectStarted();

	SetComponentTickEnabled(true);
}

void ULingeringResourceEffectOnHit::StopEffect()
{
	Super::StopEffect();

	SetComponentTickEnabled(false);
}
