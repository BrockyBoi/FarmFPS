// Fill out your copyright notice in the Description page of Project Settings.

#include "WeatherManager.h"

UWeatherManager::UWeatherManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWeatherManager::BeginPlay()
{
	Super::BeginPlay();
}

void UWeatherManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_isStorming)
	{
		_currentStormTime += DeltaTime;
		_currentStormIntensity = FMath::Lerp(0.f, _maxStormIntensity, _currentStormTime / _stormRampUpTime);
		if (_currentStormTime >= _stormRampUpTime)
		{
			SetComponentTickEnabled(false);
		}
	}
}

void UWeatherManager::CHEAT_StartStorm(const FGameplayTagContainer& resourceContainer, bool isPermanent)
{
	if (_isStorming)
	{
		EndStorm();
	}

	StartStorm(resourceContainer, isPermanent);
}

void UWeatherManager::StartStorm(const FGameplayTagContainer& resourceContainer, bool isPermanent)
{
	_isStorming = true;
	_currentStormTags = resourceContainer;
	OnStormStarted.Broadcast();
	_currentStormTime = 0.f;
	_currentStormIntensity = 0.f;
	SetComponentTickEnabled(true);
	if (!isPermanent)
	{
		GetWorld()->GetTimerManager().SetTimer(_stormTimerHandle, this, &UWeatherManager::EndStorm, _stormDuration, false);
	}
}

void UWeatherManager::EndStorm()
{
	_isStorming = false;
	OnStormEnded.Broadcast();
	_currentStormIntensity = 0.f;
	_currentStormTime = 0.f;
	SetComponentTickEnabled(false);
}