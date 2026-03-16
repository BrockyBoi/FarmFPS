// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ResourceConverterComponent.generated.h"

class UResourceInventory;
struct FCraftingData;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UResourceConverterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResourceConverterComponent();

	bool TryConvertResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft, int amountToCraft);
	bool TryConvertAllResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CanCreateResource(const UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const;
	void ConvertLimitedResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft, int amountToCraft);
	void ConvertAllResourcesPossible(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft);

	int GetMaxAmountOfResourceCanBeCrafted(const UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const;
};
