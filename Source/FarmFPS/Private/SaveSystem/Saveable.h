// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "SaveSystem/FarmFPSSaveGame.h"

// UE
#include "CoreMinimal.h"
#include "UObject/Interface.h"

// Generated
#include "Saveable.generated.h"

UINTERFACE(MinimalAPI)
class USaveable : public UInterface
{
	GENERATED_BODY()
};

class FARMFPS_API ISaveable
{
	GENERATED_BODY()

public:
	virtual void OnGameLoaded(UFarmFPSSaveGame* saveGame) = 0;
};