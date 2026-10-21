// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Generated
#include "MeleeableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UMeleeableInterface : public UInterface
{
	GENERATED_BODY()
};

class IMeleeableInterface
{
	GENERATED_BODY()

public:
	virtual void MeleeObject(int damage) = 0;
};


