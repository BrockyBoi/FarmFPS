// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

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

	bool TryConvertResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, FCraftingData& recipeToCraft);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
