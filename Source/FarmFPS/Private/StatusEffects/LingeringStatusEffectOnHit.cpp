// Fill out your copyright notice in the Description page of Project Settings.

#include "LingeringStatusEffectOnHit.h"

// Brock
#include "EffectManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Plants/Crop.h"

LingeringStatusEffectOnHit::LingeringStatusEffectOnHit(FGameplayTag resourceType, APlant* plant, float lingerDuration, float resourcesPerSecond) : OnCropHitStatusEffect(resourceType, plant)
{
	_lingerDuration = lingerDuration;
	_resourcePerSecond = resourcesPerSecond;
}

void LingeringStatusEffectOnHit::TickEffect(float deltaTime)
{
	OnCropHitStatusEffect::TickEffect(deltaTime);

	_currentTimeElapsed += deltaTime;

	if (ensure(_cropToAffect.IsValid()))
	{
		_cropToAffect->AddResource(_resourceType, _resourcePerSecond* deltaTime);
	}

	if (_currentTimeElapsed >= _lingerDuration)
	{
		StopEffect();
	}
}

void LingeringStatusEffectOnHit::StartEffect()
{
	OnCropHitStatusEffect::StartEffect();

	UEffectManager* effectManager = FarmFPSUtilities::GetEffectManager(_cropToAffect.Get());
	if (ensure(IsValid(effectManager)))
	{
		effectManager->AddLingeringEffect(*this);
	}
}

void LingeringStatusEffectOnHit::OnEffectStarted()
{
	OnCropHitStatusEffect::OnEffectStarted();
}

void LingeringStatusEffectOnHit::StopEffect()
{
	UEffectManager* effectManager = FarmFPSUtilities::GetEffectManager(_cropToAffect.Get());
	if (IsValid(effectManager))
	{
		effectManager->RemoveLingeringEffect(*this);
	}

	OnCropHitStatusEffect::StopEffect();
}
