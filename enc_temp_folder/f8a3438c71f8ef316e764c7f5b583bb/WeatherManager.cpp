// Fill out your copyright notice in the Description page of Project Settings.

#include "WeatherManager.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "StormCloudTarget.h"

UWeatherManager::UWeatherManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWeatherManager::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycleManager)))
	{
		dayNightCycleManager->OnDayBegin.AddUObject(this, &UWeatherManager::OnDayBegin);
		dayNightCycleManager->OnDayEnd.AddUObject(this, &UWeatherManager::OnDayEnd);
	}
}

void UWeatherManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycleManager))
	{
		dayNightCycleManager->OnDayBegin.RemoveAll(this);
		dayNightCycleManager->OnDayEnd.RemoveAll(this);
	}
}

void UWeatherManager::CheckIfShouldSpawnStormCloud(float deltaTime)
{
	if (_isStormCloudInScene)
	{
		return;
	}

	float randomChance = FMath::RandRange(0.f, 1.f);
	if (randomChance > _cloudSpawnChancePerSecond.GetModifiedValue(this) * deltaTime)
	{
		return;
	}

	SpawnStormCloud();
}

void UWeatherManager::OnDayBegin()
{
	SetComponentTickEnabled(!FMath::IsNearlyZero(_cloudSpawnChancePerSecond.GetModifiedValue(this)));
}

void UWeatherManager::OnDayEnd()
{
	SetComponentTickEnabled(false);
}

void UWeatherManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!_isStormCloudInScene)
	{
		CheckIfShouldSpawnStormCloud(DeltaTime);
	}

	if (_isStorming)
	{
		_currentStormTime += DeltaTime;
		_currentStormIntensity = FMath::Lerp(0.f, _maxStormIntensity.GetModifiedValue(this), _currentStormTime / _stormRampUpTime);
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

void UWeatherManager::SpawnStormCloud()
{
	if (_isStormCloudInScene)
	{
		return;
	}

	if (ensure(IsValid(_stormCloudClass)))
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AStormCloudTarget* stormCloud = GetWorld()->SpawnActor<AStormCloudTarget>(_stormCloudClass, _stormCloudSpawnLocation, FRotator::ZeroRotator, spawnParams);
		if (ensure(IsValid(stormCloud)))
		{
			stormCloud->SetDirection(FVector::RightVector);
			_isStormCloudInScene = true;
		}
	}
}

void UWeatherManager::StormCloudShot(const FGameplayTag& resourceTag)
{
	_isStormCloudInScene = false;
	StartStorm(resourceTag, false);
}

void UWeatherManager::StormCloudDisappeared()
{
	_isStormCloudInScene = false;
}

void UWeatherManager::StartStorm(const FGameplayTag& resourceTag, bool isPermanent)
{
	StartStorm(FGameplayTagContainer(resourceTag), isPermanent);
}

void UWeatherManager::StartStorm(const FGameplayTagContainer& resourceContainer, bool isPermanent)
{
	_isStorming = true;
	_currentStormTags = resourceContainer;
	OnStormStarted.Broadcast();
	OnStormStartedDynamic.Broadcast(resourceContainer);
	_currentStormTime = 0.f;
	_currentStormIntensity = 0.f;
	if (!isPermanent)
	{
		GetWorld()->GetTimerManager().SetTimer(_stormTimerHandle, this, &UWeatherManager::EndStorm, _stormDuration.GetModifiedValue(this), false);
	}
}

void UWeatherManager::EndStorm()
{
	_isStorming = false;
	OnStormEnded.Broadcast();
	OnStormEndedDynamic.Broadcast(_currentStormTags);
	_currentStormIntensity = 0.f;
	_currentStormTime = 0.f;

	_currentStormTags.Reset();
}