// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "BaseModifiableValueData.generated.h"

USTRUCT(BlueprintType)
struct FBaseModifiableValueData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifiedFloatValue BaseModifiedValue;
};
