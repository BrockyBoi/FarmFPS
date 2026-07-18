// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EStatusEffectType
{
	Arc,
	GiveResourceOverTime
};

/**
 * 
 */
class StatusEffect
{
public:
	StatusEffect();
	StatusEffect(EStatusEffectType statusEffectType);

	virtual void StartEffect();

	virtual void TickEffect(float deltaTime);

	EStatusEffectType GetStatusEffectType() const { return _statusEffectType; }

	int32 ID;

	bool _canStack = false;

	EStatusEffectType _statusEffectType;

	FORCEINLINE bool operator==(const StatusEffect& other) const
	{
		return ID == other.ID;
	}
};
