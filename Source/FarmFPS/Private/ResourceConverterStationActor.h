// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Brock
#include "CraftingData.h"
#include "InputOutputStationActor.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "ResourceConverterStationActor.generated.h"

class UAutomaticResourceTransferPoint;
class UResourceConverterComponent;
class UResourceInventory;

UCLASS()
class AResourceConverterStationActor : public AInputOutputStationActor
{
	GENERATED_BODY()
	
public:	
	AResourceConverterStationActor();
	void TryConvertAllResources();
	void TryConvertLimitedAmount(int amountToCraft);

protected:
	virtual void BeginPlay() override;

	virtual void OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, int amount) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceConverterComponent* _resourceConverter = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FCraftingData _craftingRecipe;

	UPROPERTY(EditDefaultsOnly)
	bool _automaticallyConvertResources = true;
};
