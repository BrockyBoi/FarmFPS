// Fill out your copyright notice in the Description page of Project Settings.

#include "MoonHitBox.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/TutorialScreenManager.h"

UMoonHitBox::UMoonHitBox()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMoonHitBox::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycleManager)))
	{
		_hitsLeft = _hitsNeeded;
		dayNightCycleManager->OnDayEnd.AddUObject(this, &UMoonHitBox::OnNightStart);
	}
}

void UMoonHitBox::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
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
	UTutorialScreenManager* tutorialManager = UFarmFPSUtilities::GetTutorialScreenManager(this);
	if (ensure(IsValid(tutorialManager)) && !tutorialManager->HasShownTutorialScreen(ETutorialScreenType::FirstUpgradePurchase))
	{
		return;
	}

	UDayNightCycleManager* dayNightCycleManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (_hitsLeft > 0 && IsValid(dayNightCycleManager) && dayNightCycleManager->GetCurrentDayState() == EDayState::MidNight)
	{
		_hitsLeft--;
		if (_hitsLeft <= 0)
		{
			dayNightCycleManager->TransitionToNextDay();
		}
	}
}
