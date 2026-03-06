// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


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
	// Sets default values for this actor's properties
	AResourceConverterStationActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UAutomaticResourceTransferPoint* _resourceInputPoint = nullptr;

	UPROPERTY(EditAnywhere)
	UAutomaticResourceTransferPoint* _resourceOutputPoint = nullptr;

	UPROPERTY(EditAnywhere)
	UResourceInventory* _inputInventory = nullptr;

	UPROPERTY(EditAnywhere)
	UResourceInventory* _outputInventory = nullptr;

	UPROPERTY(EditAnywhere)
	UResourceConverterComponent _resourceConverter = nullptr;
};
