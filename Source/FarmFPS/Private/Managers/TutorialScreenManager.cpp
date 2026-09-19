// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialScreenManager.h"

// Brock
#include "Interactables/BreadOven.h"
#include "Interactables/BreadStand.h"
#include "Interactables/PurchaseLocation.h"
#include "Misc/TutorialShineActor.h"
#include "Plants/Crop.h"
#include "Plants/Plant.h"
#include "Resources/ResourcePickupActor.h"
#include "Resources/ResourceTypeTag.h"
#include "SaveSystem/SaveGameManager.h"

// Shooter
#include "ShooterCharacter.h"
#include "ShooterWeapon.h"
#include "WeaponTypeTag.h"

// UE
#include "Kismet/GameplayStatics.h"

UTutorialScreenManager::UTutorialScreenManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;
}

void UTutorialScreenManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	_timeElapsedSinceTutorialOpen += DeltaTime;
	if (!_allowTutorialScreenClose && _timeElapsedSinceTutorialOpen >= _timeBeforeTutorialCanBeClosed)
	{
		AllowTutorialScreenToClose();
	}
}

void UTutorialScreenManager::BeginPlay()
{
	Super::BeginPlay();

	APlant::OnFullyGrown.AddUObject(this, &UTutorialScreenManager::OnPlantFullyGrown);
	AResourcePickupActor::OnCollected.AddUObject(this, &UTutorialScreenManager::OnResourceCollected);
	AResourcePickupActor::OnResourceSpawned.AddUObject(this, &UTutorialScreenManager::OnResourceSpawned);
	ABreadOven::OnStartConvertingResources.AddUObject(this, &UTutorialScreenManager::OnStartBakingBread);
	ABreadOven::OnBreadSpawned.AddUObject(this, &UTutorialScreenManager::OnFirstBreadSpawned);
	ABreadStand::OnBreadAddedToStand.AddUObject(this, &UTutorialScreenManager::OnBreadAddedToStand);
	UDayNightCycleManager::OnDayStateChange.AddUObject(this, &UTutorialScreenManager::OnDayNightCycleStateChanged);
	ACrop::OnCropPlanted.AddUObject(this, &UTutorialScreenManager::OnCropPlanted);
	UPurchaseLocation::StaticOnPurchaseSuccess.AddUObject(this, &UTutorialScreenManager::OnFirstUpgradePurchased);

	if (ensure(IsValid(_tutorialShineObjectClass)))
	{
		_tutorialShineObject = GetWorld()->SpawnActor<ATutorialShineActor>(_tutorialShineObjectClass, FTransform::Identity);
		HideTutorialShineObject();
	}

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGameManager)))
	{
		saveGameManager->OnLoadGameData.AddUObject(this, &UTutorialScreenManager::OnGameLoaded);
	}

	AShooterCharacter* player = Cast<AShooterCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
	if (ensure(IsValid(player)))
	{
		player->OnNewWeaponAdded.AddUObject(this, &UTutorialScreenManager::OnWeaponCollected);
	}

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UTutorialScreenManager::OnLevelReady), .5f, false);
}

void UTutorialScreenManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	APlant::OnFullyGrown.RemoveAll(this);
	AShooterWeapon::OnWeaponCollected.RemoveAll(this);
	AResourcePickupActor::OnCollected.RemoveAll(this);
	AResourcePickupActor::OnResourceSpawned.RemoveAll(this);
	ABreadOven::OnStartConvertingResources.RemoveAll(this);
	ABreadOven::OnBreadSpawned.RemoveAll(this);
	ABreadStand::OnBreadAddedToStand.RemoveAll(this);
	UDayNightCycleManager::OnDayStateChange.RemoveAll(this);
	ACrop::OnCropPlanted.RemoveAll(this);
	UPurchaseLocation::StaticOnPurchaseSuccess.RemoveAll(this);

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGameManager))
	{
		saveGameManager->OnLoadGameData.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool UTutorialScreenManager::HasShownTutorialScreen(ETutorialScreenType tutorialScreenType) const
{
	return (_shownTutorialScreensMap.Contains(tutorialScreenType) && _shownTutorialScreensMap[tutorialScreenType]) || !_shouldShowTutorials;
}

void UTutorialScreenManager::PressCloseScreen()
{}

void UTutorialScreenManager::PressStopShowingTutorials()
{
	_shouldShowTutorials = false;
	HideTutorialShineObject();
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

bool UTutorialScreenManager::HasPlayerCompletedBasicTutorial() const
{
	// Tutorial is over when either the first loaf of bread has been given, or if the player simply opts out of tutorials
	return HasShownTutorialScreen(ETutorialScreenType::GiveBreadToStand) || !_shouldShowTutorials;
}

void UTutorialScreenManager::OnWeaponCollected(AShooterWeapon* weapon)
{
	const FGameplayTag& resourceType = weapon->GetWeaponResourceType();
	if (resourceType == ResourceTypeTag::Light && AttemptShowScreen(ETutorialScreenType::PickUpLightGun))
	{
		if (ensure(_waterPistol.IsValid()))
		{
			_waterPistol->SetActorEnableCollision(true);
			_waterPistol->SetActorHiddenInGame(false);
			TutorialShineObjectOnActor(_waterPistol.Get(), EShineSize::Small);
		}
	}
	else if (resourceType == ResourceTypeTag::Water && AttemptShowScreen(ETutorialScreenType::PickUpWaterGun))
	{
		if (ensure(_wheatPistol.IsValid()))
		{
			_wheatPistol->SetActorEnableCollision(true);
			_wheatPistol->SetActorHiddenInGame(false);
			TutorialShineObjectOnActor(_wheatPistol.Get(), EShineSize::Small);
		}
	}
	else if (resourceType == ResourceTypeTag::Wheat && AttemptShowScreen(ETutorialScreenType::PickUpSeedGun))
	{
		AShooterCharacter* player = Cast<AShooterCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
		if (ensure(IsValid(player)))
		{
			player->ForceWeaponEquip(WeaponTypeTag::WheatSeedPistol);
		}

		TArray<AActor*> wheatPlots;
		UGameplayStatics::GetAllActorsOfClass(this, _wheatPlotBPClass, wheatPlots);
		for (AActor* farmPlot : wheatPlots)
		{
			if (ensure(IsValid(farmPlot)) && farmPlot->ActorHasTag(FName(TEXT("StartingPlot"))))
			{
				TutorialShineObjectOnActor(farmPlot, EShineSize::Medium);
				break;
			}
		}
	}
}

void UTutorialScreenManager::OnStartBakingBread()
{
	if (AttemptShowScreen(ETutorialScreenType::ThrowResourceIntoOven))
	{
		HideTutorialShineObject();
	}
}

void UTutorialScreenManager::OnFirstBreadSpawned(AActor* breadActor)
{
	if (AttemptShowScreen(ETutorialScreenType::FirstBreadSpawned))
	{
		TutorialShineObjectFollowActor(breadActor, EShineSize::Small);
	}
}

void UTutorialScreenManager::OnBreadAddedToStand()
{
	if (AttemptShowScreen(ETutorialScreenType::GiveBreadToStand))
	{
		HideTutorialShineObject();
		UDayNightCycleManager* dayNightManager = UFarmFPSUtilities::GetDayNightCycleManager(this);
		if (ensure(IsValid(dayNightManager)))
		{
			dayNightManager->ForceSetTimeLeft(5.f);
		}
	}
}

void UTutorialScreenManager::OnDayNightCycleStateChanged(EDayState newDayState)
{
	if (newDayState == EDayState::MidNight && AttemptShowScreen(ETutorialScreenType::FirstNight) && ensure(IsValid(_upgradePointBPClass)))
	{
		TArray<AActor*> upgradePoints;
		UGameplayStatics::GetAllActorsOfClass(this, _upgradePointBPClass, upgradePoints);
		for (AActor* upgradePoint : upgradePoints)
		{
			if (ensure(IsValid(upgradePoint)) && upgradePoint->ActorHasTag(FName(TEXT("StartingUpgradePoint"))))
			{
				TutorialShineObjectOnActor(upgradePoint, EShineSize::Large);
				break;
			}
		}
	}
	else if (newDayState == EDayState::NightTransitionToDay)
	{
		AttemptShowScreen(ETutorialScreenType::MoonGoDown);
	}
}

void UTutorialScreenManager::OnResourceCollected(const FGameplayTag& resourceType)
{
	if (resourceType == ResourceTypeTag::Bread)
	{
		if (AttemptShowScreen(ETutorialScreenType::ObtainFirstBread))
		{
			AActor* breadStand = UGameplayStatics::GetActorOfClass(this, ABreadStand::StaticClass());
			TutorialShineObjectOnActor(breadStand, EShineSize::Large);
		}
	}
	else
	{
		if (AttemptShowScreen(ETutorialScreenType::PickUpFirstResource))
		{
			TArray<AActor*> breadOvens;
			UGameplayStatics::GetAllActorsOfClass(this, ABreadOven::StaticClass(), breadOvens);
			for (AActor* breadOven : breadOvens)
			{
				if (ensure(IsValid(breadOven)) && breadOven->ActorHasTag(FName(TEXT("StartingOven"))))
				{
					TutorialShineObjectOnActor(breadOven, EShineSize::Medium);
					break;
				}
			}
		}

	}
}

void UTutorialScreenManager::OnCropPlanted(ACrop* crop)
{
	if (ensure(IsValid(crop)) && AttemptShowScreen(ETutorialScreenType::PlantFirstSeed))
	{
		_firstCrop = crop;
		AShooterCharacter* player = Cast<AShooterCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
		if (ensure(IsValid(player)))
		{
			player->ForceWeaponEquip(WeaponTypeTag::LightRifle);
		}

		crop->OnPlantResourceFull.AddUObject(this, &UTutorialScreenManager::OnFirstCropResourceFilled);
		crop->SetIsInvincible(true);

		TutorialShineObjectOnActor(crop, EShineSize::Small);
	}
}

void UTutorialScreenManager::OnPlantFullyGrown()
{
	//if (AttemptShowScreen(ETutorialScreenType::FullyGrowCrop))
	//{
	//	HideTutorialShineObject();

	//}
}

void UTutorialScreenManager::OnResourceSpawned(AResourcePickupActor* resourcePickupActor)
{
	if (!IsValid(resourcePickupActor))
	{
		return;
	}

	if (!_hasFirstWheatSpawned && resourcePickupActor->GetResourceType() == ResourceTypeTag::Wheat)
	{
		_hasFirstWheatSpawned = true;
		TutorialShineObjectFollowActor(resourcePickupActor, EShineSize::Small);
		return;
	}

	if (!_hasFirstBreadSpawned && resourcePickupActor->GetResourceType() == ResourceTypeTag::Bread)
	{
		_hasFirstBreadSpawned = true;
		TutorialShineObjectFollowActor(resourcePickupActor, EShineSize::Small);
		return;
	}
}

void UTutorialScreenManager::OnLevelReady()
{
	if (AttemptShowScreen(ETutorialScreenType::OnPlayerSpawned))
	{
		TArray<AActor*> weaponPickups;
		UGameplayStatics::GetAllActorsOfClass(this, _weaponPickupBPClass, weaponPickups);
		for (AActor* weaponPickup : weaponPickups)
		{
			if (ensure(IsValid(weaponPickup)))
			{
				if (weaponPickup->ActorHasTag(FName(TEXT("LightRifle"))))
				{
					TutorialShineObjectOnActor(weaponPickup, EShineSize::Small);
				}
				else if (weaponPickup->ActorHasTag(FName(TEXT("WaterPistol"))))
				{
					_waterPistol = weaponPickup;
					_waterPistol->SetActorHiddenInGame(true);
					_waterPistol->SetActorEnableCollision(false);
				}
				else if (weaponPickup->ActorHasTag(FName(TEXT("WheatPistol"))))
				{
					_wheatPistol = weaponPickup;
					_wheatPistol->SetActorHiddenInGame(true);
					_wheatPistol->SetActorEnableCollision(false);
				}
			}
		}
	}
}

void UTutorialScreenManager::OnFirstUpgradePurchased()
{
	if (AttemptShowScreen(ETutorialScreenType::FirstUpgradePurchase))
	{
		HideTutorialShineObject();
	}
}

bool UTutorialScreenManager::AttemptShowScreen(ETutorialScreenType screenToShow)
{
	if (GetShouldShowTutorials() && !HasShownTutorialScreen(screenToShow))
	{
		_shownTutorialScreensMap.Add(screenToShow, true);
		OnTutorialScreenShown.Broadcast(screenToShow);

		_allowTutorialScreenClose = false;

		SetComponentTickEnabled(true);
		return true;
	}

	return false;
}

void UTutorialScreenManager::HideTutorialShineObject()
{
	if (ensure(_tutorialShineObject.IsValid()))
	{
		_tutorialShineObject->Hide();
	}
}

void UTutorialScreenManager::TutorialShineObjectOnActor(AActor* actorToFollow, EShineSize shineSize)
{
	if (ensure(_tutorialShineObject.IsValid()) && ensure(IsValid(actorToFollow)))
	{
		_tutorialShineObject->Show(actorToFollow->GetActorLocation(), shineSize);
	}
}

void UTutorialScreenManager::TutorialShineObjectFollowActor(AActor* actorToFollow, EShineSize shineSize)
{
	if (ensure(_tutorialShineObject.IsValid()) && ensure(IsValid(actorToFollow)))
	{
		_tutorialShineObject->FollowActor(actorToFollow, shineSize);
	}
}

void UTutorialScreenManager::AllowTutorialScreenToClose()
{
	_allowTutorialScreenClose = true;
	OnAllowTutorialScreenToClose.Broadcast();
	_timeElapsedSinceTutorialOpen = 0.f;

	SetComponentTickEnabled(false);
}

void UTutorialScreenManager::OnFirstCropResourceFilled(const FGameplayTag& resource)
{
	if (resource == ResourceTypeTag::Light)
	{
		AttemptShowScreen(ETutorialScreenType::FillFirstCropWithLight);
		AShooterCharacter* player = Cast<AShooterCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
		if (ensure(IsValid(player)))
		{
			player->ForceWeaponEquip(WeaponTypeTag::WaterPistol);
		}
	}
	else if (resource == ResourceTypeTag::Water)
	{
		AttemptShowScreen(ETutorialScreenType::FullyGrowCrop);
		AShooterCharacter* player = Cast<AShooterCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
		if (ensure(IsValid(player)))
		{
			player->StopForcingWeaponEquip();
		}

		if (ensure(_firstCrop.IsValid()))
		{
			_firstCrop->SetIsInvincible(false);
			_firstCrop->OnPlantResourceFull.RemoveAll(this);
		}
	}
}

bool UTutorialScreenManager::CanCloseTutorialScreen() const
{
	return _allowTutorialScreenClose;
}
