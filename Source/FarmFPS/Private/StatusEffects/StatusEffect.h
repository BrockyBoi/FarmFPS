// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class StatusEffect
{
public:
	StatusEffect();
	StatusEffect(bool isLingeringEffect);

	virtual void StartEffect();

	virtual void TickEffect(float deltaTime);

	bool _isLingeringEffect = false;
};
