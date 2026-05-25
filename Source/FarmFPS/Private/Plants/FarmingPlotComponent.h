// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "FarmingPlotComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UFarmingPlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFarmingPlotComponent();

	FGameplayTagContainer& GetAllowedSeedTypes() { return _allowedSeedTypes; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."))
	FGameplayTagContainer _allowedSeedTypes;
};
