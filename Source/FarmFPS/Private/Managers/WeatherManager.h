// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "WeatherManager.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWeatherChangeEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChangeDynamicEvent, FGameplayTagContainer, stormTags);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeatherManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeatherManager();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CHEAT_StartStorm(const FGameplayTagContainer& resourceContainer, bool isPermanent = false);
	void StartStorm(const FGameplayTagContainer& resourceContainer, bool isPermanent = false);
	void EndStorm();

	const FGameplayTagContainer& GetCurrentStormTags() const { return _currentStormTags; }
	float GetCurrentStormIntensity() const { return _currentStormIntensity; }
	bool IsStorming() const { return _isStorming; }

	FOnWeatherChangeEvent OnStormStarted;
	FOnWeatherChangeEvent OnStormEnded;

protected:
	virtual void BeginPlay() override;

	bool _isStorming = false;
	float _currentStormTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storm")
	FModifiedFloatValue _stormDuration = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storm")
	float _stormRampUpTime = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storm")
	FModifiedFloatValue _maxStormIntensity = 5.f;

	float _currentStormIntensity = 0.f;

	FTimerHandle _stormTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Storm")
	FGameplayTagContainer _currentStormTags;
};
