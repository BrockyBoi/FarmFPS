// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "StatusEffect.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class ACrop;
class APlant;

class OnCropHitStatusEffect : public StatusEffect 
{
public:
	OnCropHitStatusEffect();
	OnCropHitStatusEffect(EStatusEffectType statusEffectType, FGameplayTag resourceType, APlant* plant);

protected:
	virtual void OnEffectStarted();
	virtual void StopEffect();

	virtual void OnCropDestroyed();

	FModifiedFloatValue _procChanceOnHit = .15f;
	TWeakObjectPtr<ACrop> _cropToAffect = nullptr;
	FGameplayTag _resourceType;
};
