// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ProjectileDataAsset.h"

// UE
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraComponent.h"

// Generated
#include "CropProjectileDataAsset.generated.h"

UCLASS()
class UCropProjectileDataAsset : public UProjectileDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Audio")
	TSoftObjectPtr<USoundBase> OnPlantSeedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Audio")
	TSoftObjectPtr<USoundBase> OnFailToPlantSeedSound;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannelToCheck;
};
