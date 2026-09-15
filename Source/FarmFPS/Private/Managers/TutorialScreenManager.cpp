// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialScreenManager.h"

// Brock
#include "Plants/Crop.h"
#include "Plants/Plant.h"
#include "Interactables/BreadOven.h"
#include "Interactables/BreadStand.h"
#include "Resources/ResourcePickupActor.h"
#include "Resources/ResourceTypeTag.h"
#include "SaveSystem/SaveGameManager.h"
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
	ACrop::OnCropPlanted.AddUObject(this, &UTutorialScreenManager::OnCropPlanted);

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGameManager)))
	{
		saveGameManager->OnLoadGameData.AddUObject(this, &UTutorialScreenManager::OnGameLoaded);
	}

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UTutorialScreenManager::AttemptShowScreen, ETutorialScreenType::OnPlayerSpawned), .5f, false);
}

void UTutorialScreenManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	APlant::OnFullyGrown.RemoveAll(this);
	AShooterWeapon::OnWeaponCollected.RemoveAll(this);
	AResourcePickupActor::OnCollected.RemoveAll(this);
	ABreadOven::OnIngredientAddedToOven.RemoveAll(this);
	ABreadStand::OnBreadAddedToStand.RemoveAll(this);
	UDayNightCycleManager::OnDayStateChange.RemoveAll(this);
	ACrop::OnCropPlanted.RemoveAll(this);

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGameManager))
	{
		saveGameManager->OnLoadGameData.RemoveAll(this);
	}

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

FTutorialSaveGameData UTutorialScreenManager::GetTutorialSaveGameData() const
{
	FTutorialSaveGameData saveData;
	saveData.ShouldShowTutorials = _shouldShowTutorials;

	TArray<ETutorialScreenType> tutorialsShown;
	TArray<uint8> tutorialsShownInts;
	_shownTutorialScreensMap.GetKeys(tutorialsShown);
	for (auto tutorial : tutorialsShown)
	{
		tutorialsShownInts.Add((uint8)tutorial);
	}
	saveData.TutorialsShown = tutorialsShownInts;

	return saveData;
}

void UTutorialScreenManager::OnGameLoaded(UFarmFPSSaveGame* saveGame)
{
	if (ensure(saveGame))
	{
		_shouldShowTutorials = saveGame->GetTutorialSaveData().ShouldShowTutorials;
		if (!_shouldShowTutorials)
		{
			OnTutorialScreenForceClosed.Broadcast();
		}

		for (uint8 tutorialType : saveGame->GetTutorialSaveData().TutorialsShown)
		{
			_shownTutorialScreensMap.Add((ETutorialScreenType)tutorialType, true);
		}
	}
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

void UTutorialScreenManager::OnCropPlanted(const FGameplayTag& resourceType)
{
	AttemptShowScreen(ETutorialScreenType::PlantFirstSeed);
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
