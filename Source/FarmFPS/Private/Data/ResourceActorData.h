// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Plants/Crop.h"
#include "ShooterProjectile.h"
#include "Resources/ResourcePickupActor.h"

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
	TSoftClassPtr<AResourcePickupActor> ResourceActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<AShooterProjectile> ProjectileActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<ACrop> CropActorClass;
};
