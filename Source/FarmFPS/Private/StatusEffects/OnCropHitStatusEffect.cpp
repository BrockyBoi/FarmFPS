// Fill out your copyright notice in the Description page of Project Settings.

#include "OnCropHitStatusEffect.h"

// Brock
#include "Plants/Crop.h"

OnCropHitStatusEffect::OnCropHitStatusEffect()
{
}

OnCropHitStatusEffect::OnCropHitStatusEffect(FGameplayTag resourceType, APlant* plant)
{
	_resourceType = resourceType;
	_cropToAffect = Cast<ACrop>(plant);
	ensure(_cropToAffect.IsValid());
}

void OnCropHitStatusEffect::TickEffect(float deltaTime)
{
	// Do stuff presumably
}

void OnCropHitStatusEffect::OnEffectStarted()
{

}

void OnCropHitStatusEffect::StopEffect()
{
	_cropToAffect = nullptr;
}
