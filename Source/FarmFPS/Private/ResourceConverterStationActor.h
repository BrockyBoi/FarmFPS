// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Brock
#include "CraftingData.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "ResourceConverterStationActor.generated.h"

class UAutomaticResourceTransferPoint;
class UResourceConverterComponent;
class UResourceInventory;

UCLASS()
class AResourceConverterStationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AResourceConverterStationActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnInputInventoryResourceCountChanged(EResourceType resourceType, int amount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAutomaticResourceTransferPoint* _resourceInputPoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAutomaticResourceTransferPoint* _resourceOutputPoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceInventory* _inputInventory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceInventory* _outputInventory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceConverterComponent* _resourceConverter = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FCraftingData _craftingRecipe;
};
