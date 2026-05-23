// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Crop.h"
#include "ShooterProjectile.h"
#include "ResourcePickupActor.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "ResourceActorData.generated.h"

USTRUCT(BlueprintType)
struct FResourceActorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "CropResourceType,ResourceType"))
	FGameplayTag ResourceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AResourcePickupActor> ResourceActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AShooterProjectile> ProjectileActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ACrop> CropActorClass;
};
