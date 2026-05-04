// Fill out your copyright notice in the Description page of Project Settings.

#include "MoonHitBox.h"

// Brock
#include "DayNightCycleManager.h"
#include "FarmFPSUtilities.h"

UMoonHitBox::UMoonHitBox()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMoonHitBox::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycleManager = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycleManager)))
	{
		_hitsLeft = _hitsNeeded;
		dayNightCycleManager->OnDayEnd.AddUObject(this, &UMoonHitBox::OnNightStart);
	}
}

void UMoonHitBox::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycleManager = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycleManager))
	{
		dayNightCycleManager->OnDayEnd.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UMoonHitBox::OnNightStart()
{
	_hitsLeft = _hitsNeeded;
}

void UMoonHitBox::HitMoon()
{
	UDayNightCycleManager* dayNightCycleManager = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (_hitsLeft > 0 && IsValid(dayNightCycleManager) && dayNightCycleManager->GetCurrentDayState() == EDayState::MidNight)
	{
		_hitsLeft--;
		if (_hitsLeft <= 0)
		{
			dayNightCycleManager->TransitionToNextDay();
		}
	}
}
