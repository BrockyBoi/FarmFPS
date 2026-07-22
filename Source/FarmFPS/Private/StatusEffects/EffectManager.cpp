// Fill out your copyright notice in the Description page of Project Settings.

#include "EffectManager.h"

UEffectManager::UEffectManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEffectManager::BeginPlay()
{
	Super::BeginPlay();
}

void UEffectManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = 0; i < _lingeringStatusEffects.Num(); ++i)
	{
		StatusEffect* effect = _lingeringStatusEffects[i];
		if (ensure(effect != nullptr))
		{
			effect->TickEffect(DeltaTime);
		}
	}
}

void UEffectManager::AddLingeringEffect(StatusEffect* statusEffect)
{
	_lingeringStatusEffects.AddUnique(statusEffect);

	if (_lingeringStatusEffects.Num() == 1)
	{
		SetComponentTickEnabled(true);
	}
}

void UEffectManager::RemoveLingeringEffect(StatusEffect* statusEffect)
{
	if (_lingeringStatusEffects.Contains(statusEffect))
	{
		_lingeringStatusEffects.Remove(statusEffect);

		if (_lingeringStatusEffects.Num() == 0)
		{
			SetComponentTickEnabled(false);
		}
	}
}

void UEffectManager::ClearAllStatusEffects()
{
	_lingeringStatusEffects.Empty();
	SetComponentTickEnabled(false);
}

bool UEffectManager::HasStatusEffectType(EStatusEffectType statusEffectType) const
{
	for (StatusEffect* statusEffect : _lingeringStatusEffects)
	{
		if (statusEffect != nullptr && statusEffect->GetStatusEffectType() == statusEffectType)
		{
			return true;
		}
	}

	return false;
}

