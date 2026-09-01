// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialScreenManager.h"

// Brock
#include "Plants/Plant.h"
#include "Interactables/BreadOven.h"
#include "Interactables/BreadStand.h"
#include "Resources/ResourcePickupActor.h"
#include "Resources/ResourceTypeTag.h"
#include "ShooterWeapon.h"

UTutorialScreenManager::UTutorialScreenManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTutorialScreenManager::BeginPlay()
{
	Super::BeginPlay();

	APlant::OnFullyGrown.AddUObject(this, &UTutorialScreenManager::OnPlantFullyGrown);
	AShooterWeapon::OnWeaponCollected.AddUObject(this, &UTutorialScreenManager::OnWeaponCollected);
	AResourcePickupActor::OnCollected.AddUObject(this, &UTutorialScreenManager::OnResourceCollected);
	ABreadOven::OnIngredientAddedToOven.AddUObject(this, &UTutorialScreenManager::OnIngredientAddedToOven);
	ABreadStand::OnBreadAddedToStand.AddUObject(this, &UTutorialScreenManager::OnBreadAddedToStand);
	UDayNightCycleManager::OnDayStateChange.AddUObject(this, &UTutorialScreenManager::OnDayNightCycleStateChanged);
}

void UTutorialScreenManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	APlant::OnFullyGrown.RemoveAll(this);
	AShooterWeapon::OnWeaponCollected.RemoveAll(this);
	AResourcePickupActor::OnCollected.RemoveAll(this);
	ABreadOven::OnIngredientAddedToOven.RemoveAll(this);
	ABreadStand::OnBreadAddedToStand.RemoveAll(this);
	UDayNightCycleManager::OnDayStateChange.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

bool UTutorialScreenManager::HasShownTutorialScreen(ETutorialScreenType tutorialScreenType) const
{
	return _shownTutorialScreensMap.Contains(tutorialScreenType) && _shownTutorialScreensMap[tutorialScreenType];
}

void UTutorialScreenManager::PressCloseScreen()
{}

void UTutorialScreenManager::PressStopShowingTutorials()
{
	_shouldShowTutorials = false;
}

void UTutorialScreenManager::OnWeaponCollected(const FGameplayTag& resourceType)
{
	if (resourceType == ResourceTypeTag::Light)
	{
		AttemptShowScreen(ETutorialScreenType::PickUpLightGun);
	}
	else if (resourceType == ResourceTypeTag::Water)
	{
		AttemptShowScreen(ETutorialScreenType::PickUpWaterGun);
	}
	else if (resourceType == ResourceTypeTag::Wheat)
	{
		AttemptShowScreen(ETutorialScreenType::PickUpSeedGun);
	}
}

void UTutorialScreenManager::OnIngredientAddedToOven(const FGameplayTag& ingredientType)
{
	AttemptShowScreen(ETutorialScreenType::ThrowResourceIntoOven);
}

void UTutorialScreenManager::OnBreadAddedToStand()
{
	AttemptShowScreen(ETutorialScreenType::ObtainFirstBread);
}

void UTutorialScreenManager::OnDayNightCycleStateChanged(EDayState newDayState)
{
	if (newDayState == EDayState::MidNight)
	{
		AttemptShowScreen(ETutorialScreenType::FirstNight);
	}
	else if (newDayState == EDayState::WaitingForTradeOff)
	{
		AttemptShowScreen(ETutorialScreenType::FirstNight);
	}
}

void UTutorialScreenManager::OnResourceCollected(const FGameplayTag& resourceType)
{
	AttemptShowScreen(ETutorialScreenType::PickUpFirstResource);

	if (resourceType == ResourceTypeTag::Bread)
	{
		AttemptShowScreen(ETutorialScreenType::ObtainFirstBread);
	}
}

void UTutorialScreenManager::OnPlantFullyGrown()
{
	AttemptShowScreen(ETutorialScreenType::FullyGrowCrop);
}

void UTutorialScreenManager::AttemptShowScreen(ETutorialScreenType screenToShow)
{
	if (GetShouldShowTutorials() && !HasShownTutorialScreen(screenToShow))
	{
		_shownTutorialScreensMap.Add(screenToShow, true);
		OnTutorialScreenShown.Broadcast(screenToShow);
	}
}
