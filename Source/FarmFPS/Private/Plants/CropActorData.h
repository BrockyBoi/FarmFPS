// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Crop.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "CropActorData.generated.h"

USTRUCT(BlueprintType)
struct FCropActorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "ResourceType."))
	FGameplayTag ResourceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ACrop> CropActorClass;
};
