// Fill out your copyright notice in the Description page of Project Settings.

#include "OnCropHitStatusEffect.h"

// Brock
#include "Plants/Crop.h"

OnCropHitStatusEffect::OnCropHitStatusEffect() : StatusEffect()
{
}

OnCropHitStatusEffect::OnCropHitStatusEffect(EStatusEffectType statusEffectType, FGameplayTag resourceType, APlant* plant) : StatusEffect(statusEffectType)
{
	_resourceType = resourceType;
	_cropToAffect = Cast<ACrop>(plant);
	if (ensure(_cropToAffect.IsValid()))
	{
		_cropToAffect->OnPlantBreak.AddRaw(this, &OnCropHitStatusEffect::OnCropDestroyed);
	}
}

void OnCropHitStatusEffect::OnEffectStarted()
{

}

void OnCropHitStatusEffect::StopEffect()
{
	if (_cropToAffect.IsValid())
	{
		_cropToAffect->OnPlantBreak.RemoveAll(this);
		_cropToAffect = nullptr;
	}
}

void OnCropHitStatusEffect::OnCropDestroyed()
{
	StopEffect();
}
