// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"

// Generated
#include "UpgradeLocationSaveData.generated.h"

USTRUCT(BlueprintType)
struct FUpgradeLocationSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FString PurchaseLocationName;

	UPROPERTY()
	int NumberOfPurchases;

	UPROPERTY()
	bool IsEnabled;
};
