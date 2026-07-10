// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "Resources/GrenadeActor.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "ReverseGrenadePickupActor.generated.h"

class APlant;

/**
 *
 */
UCLASS()
class AReverseGrenadePickupActor : public AGrenadeActor
{
	GENERATED_BODY()

public:
	AReverseGrenadePickupActor();
};
