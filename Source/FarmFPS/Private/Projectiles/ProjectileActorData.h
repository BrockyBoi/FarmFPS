// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ShooterProjectile.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "ProjectileActorData.generated.h"

USTRUCT(BlueprintType)
struct FProjectileActorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "CropResourceType,ResourceType"))
	FGameplayTag ProjectileType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AShooterProjectile> ProjectileActorClass;
};