// Fill out your copyright notice in the Description page of Project Settings.

#include "DayNightCycleManager.h"

// UE
#include "EngineUtils.h"
#include "Engine/DirectionalLight.h"

UDayNightCycleManager::UDayNightCycleManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDayNightCycleManager::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		if (IsValid(actor))
		{
			if (!_sunLight.IsValid() && actor->Tags.Contains(TEXT("Sun")))
			{
				_sunLight = *ActorItr;
			}

			if (!_moonLight.IsValid() && actor->Tags.Contains(TEXT("Moon")))
			{
				_moonLight = *ActorItr;
			}
		}
	}

	StartDay();
}


void UDayNightCycleManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetCurrentDayState() == EDayState::Day && ensure(_sunLight.IsValid()))
	{
		_timeElapsed += DeltaTime;
		float dayLength = _dayLength.GetModifiedValue(this);
		float lerpedPitch = FMath::Lerp(0.f, 180.f, _timeElapsed / dayLength) + 180.f;

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_sunLight->SetActorRotation(rotation);

		if (_timeElapsed >= dayLength)
		{
			EndDay();
		}
	}

	if (GetCurrentDayState() == EDayState::MidNight && ensure(_moonLight.IsValid()))
	{
		if (_timeElapsed >= _timeToReachPeakMoon)
		{
			return;
		}

		_timeElapsed += DeltaTime;
		float lerpedPitch = FMath::Lerp(0.f, 70.f, _timeElapsed / _timeToReachPeakMoon) + 180.f;

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_moonLight->SetActorRotation(rotation);
	}

	if (GetCurrentDayState() == EDayState::NightTransitionToDay && ensure(_moonLight.IsValid()))
	{
		if (_timeElapsed >= _timeToReachPeakMoon)
		{
			return;
		}

		_timeElapsed += DeltaTime;
		float lerpedPitch = FMath::Lerp(70.f, 200.f, _timeElapsed / _timeToReachPeakMoon) + 180.f;

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_moonLight->SetActorRotation(rotation);

		if (_timeElapsed >= _timeToReachPeakMoon)
		{
			StartDay();
		}
	}
}

void UDayNightCycleManager::TransitionToNextDay()
{
	_currentDayState = EDayState::NightTransitionToDay;
	_timeElapsed = 0.f;
}

void UDayNightCycleManager::StartDay()
{
	_currentDayState = EDayState::Day;
	if (OnDayBegin.IsBound())
	{
		OnDayBegin.Broadcast();
	}

	if (ensure(_sunLight.IsValid()) && ensure(_moonLight.IsValid()))
	{
		_sunLight->SetActorHiddenInGame(false);
		_moonLight->SetActorHiddenInGame(true);

		FRotator rotation(90.f, 0.f, 0.f);
		_sunLight->SetActorRotation(rotation);
	}
}

void UDayNightCycleManager::EndDay()
{
	_currentDayState = EDayState::MidNight;
	if (OnDayEnd.IsBound())
	{
		OnDayEnd.Broadcast();
	}

	_timeElapsed = 0.f;

	if (ensure(_sunLight.IsValid()) && ensure(_moonLight.IsValid()))
	{
		_sunLight->SetActorHiddenInGame(true);
		_moonLight->SetActorHiddenInGame(false);

		FRotator moonRotation(90.f, 0.f, 0.f);
		_moonLight->SetActorRotation(moonRotation);
	}

	FTimerHandle timerHandle;
	//GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UDayNightCycleManager::StartDay, _timeUntilAutoStartNextDay, false);
}

