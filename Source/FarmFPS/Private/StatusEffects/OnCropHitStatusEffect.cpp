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
	ensure(_cropToAffect.IsValid());
}

void OnCropHitStatusEffect::OnEffectStarted()
{

}

void OnCropHitStatusEffect::StopEffect()
{
	_cropToAffect = nullptr;
}
