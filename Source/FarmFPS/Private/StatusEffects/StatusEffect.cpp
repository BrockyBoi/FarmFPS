// Fill out your copyright notice in the Description page of Project Settings.

#include "StatusEffect.h"

StatusEffect::StatusEffect()
{
	ID = FMath::RandRange(0, TNumericLimits<int32>::Max());
}

StatusEffect::StatusEffect(EStatusEffectType statusEffectType)
{
	ID = FMath::RandRange(0, TNumericLimits<int32>::Max());
	_statusEffectType = statusEffectType;
}

void StatusEffect::StartEffect()
{}

void StatusEffect::TickEffect(float deltaTime)
{}

