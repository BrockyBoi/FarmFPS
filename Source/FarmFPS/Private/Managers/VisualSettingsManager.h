// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "VisualSettingsManager.generated.h"

UENUM(BlueprintType)
enum class EVisualsValue : uint8
{
	Bad,
	LessBad,
	Fine,
	FuckingPrimo,
	PureCinema,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UVisualSettingsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVisualSettingsManager();

	UFUNCTION(BlueprintCallable)
	void SetVisualSettings(EVisualsValue visualsValue);

	UFUNCTION(BlueprintCallable)
	void CycleVisualsPreset();

protected:
	virtual void BeginPlay() override;
	void SetResolution();
	void SetAutoSettings();

	EVisualsValue _currentVisualsPreset;
};
