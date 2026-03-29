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
		_directionalLight = *ActorItr;
		break; // Assuming you have only one main directional light
	}

	StartDay();
}


void UDayNightCycleManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!_isDayOver && ensure(_directionalLight.IsValid()))
	{
		_timeElapsed += DeltaTime;
		float dayLength = _dayLength.GetModifiedValue(this);
		float lerpedPitch = FMath::Lerp(0.f, 180.f, _timeElapsed / dayLength) + 180.f;

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_directionalLight->SetActorRotation(rotation);

		if (_timeElapsed >= dayLength)
		{
			EndDay();
		}
	}
}

void UDayNightCycleManager::StartDay()
{
	if (OnDayBegin.IsBound())
	{
		OnDayBegin.Broadcast();
	}

	if (ensure(_directionalLight.IsValid()))
	{
		FRotator rotation(90.f, 0.f, 0.f);
		_directionalLight->SetActorRotation(rotation);

		_isDayOver = false;
	}
}

void UDayNightCycleManager::EndDay()
{
	if (OnDayEnd.IsBound())
	{
		OnDayEnd.Broadcast();
	}

	_timeElapsed = 0.f;
	_isDayOver = true;

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UDayNightCycleManager::StartDay, _timeUntilAutoStartNextDay, false);
}

