// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/SaveGameManager.h"

// SaveSystem
#include "PlantSaveData.h"

// FarmFPS
#include "FarmFPSCharacter.h"
#include "ShooterPickup.h"

// Brock
#include "FarmFPSSaveGame.h"
#include "Interactables/PurchaseLocation.h"
#include "Managers/AudioManager.h"
#include "Managers/BreadRequirementManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/TutorialScreenManager.h"
#include "Plants/Plant.h"

// UE
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

USaveGameManager::USaveGameManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveGameManager::BeginPlay()
{
	Super::BeginPlay();

	UPurchaseLocation::StaticOnPurchaseSuccess.AddUObject(this, &USaveGameManager::SaveGame);

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &USaveGameManager::SaveGame);
	}

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USaveGameManager::OnPostLoadMap);
}

void USaveGameManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UPurchaseLocation::StaticOnPurchaseSuccess.RemoveAll(this);
	
	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void USaveGameManager::OnPostLoadMap(UWorld*)
{
	ResetSavableActors();

	if (HasSaveGame())
	{
		LoadGame();
	}

	if (HasSaveGameSettings())
	{
		LoadGameSettings();
	}
}

void USaveGameManager::ResetSavableActors()
{
	TArray<AActor*> upgradeLocations;
	TArray<AActor*> unlockActorLocations;
	UGameplayStatics::GetAllActorsWithInterface(this, USaveable::StaticClass(), _saveableActors);
	UGameplayStatics::GetAllActorsOfClass(this, _defaultUpgradePointBP, upgradeLocations);
	UGameplayStatics::GetAllActorsOfClass(this, _defaultUnlockActorPointBP, unlockActorLocations);
	_saveableActors.Append(upgradeLocations);
	_saveableActors.Append(unlockActorLocations);
	_saveableActors.Add(GetOwner());
}

void USaveGameManager::SaveGame()
{
	FPlayerSaveData playerSaveData;
	FTutorialSaveGameData tutorialSaveData;
	FBreadRequirementManagerSaveGameData breadRequirementSaveData;
	TArray<FUpgradeLocationSaveData> upgradeLocationSaveData;
	TArray<FWeaponPickupSaveGameData> weaponPickupSaveData;
	TArray<FPlantSaveData> plantSaveDatas;

	AFarmFPSCharacter* playerCharacter = Cast<AFarmFPSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (ensure(IsValid(playerCharacter)))
	{
		playerSaveData = playerCharacter->GetPlayerSaveData();
	}

	if (_saveableActors.Num() <= 0)
	{
		ResetSavableActors();
	}

	for (AActor* saveableActor : _saveableActors)
	{
		if (ensure(IsValid(saveableActor)))
		{
			UPurchaseLocation* purchaseLocation = Cast<UPurchaseLocation>(saveableActor->FindComponentByClass(UPurchaseLocation::StaticClass()));
			if (IsValid(purchaseLocation))
			{
				upgradeLocationSaveData.Add(purchaseLocation->GetUpgradeLocationSaveData());
				continue;
			}

			AShooterPickup* shooterPickup = Cast<AShooterPickup>(saveableActor);
			if (IsValid(shooterPickup))
			{
				weaponPickupSaveData.Add(shooterPickup->GetWeaponPickupSaveGameData());
				continue;
			}

			APlant* plant = Cast<APlant>(saveableActor);
			if (IsValid(plant))
			{
				FPlantSaveData plantSaveData = plant->GetPlantSaveData();
				if (!plantSaveData.PlantName.IsEmpty())
				{
					plantSaveDatas.Add(plantSaveData);
				}
				continue;
			}

			UTutorialScreenManager* tutorialScreenManager = saveableActor->FindComponentByClass<UTutorialScreenManager>();
			if (IsValid(tutorialScreenManager))
			{
				tutorialSaveData = tutorialScreenManager->GetTutorialSaveGameData();
			}

			UBreadRequirementManager* breadRequirementManager = saveableActor->FindComponentByClass<UBreadRequirementManager>();
			if (IsValid(breadRequirementManager))
			{
				breadRequirementSaveData = breadRequirementManager->GetBreadRequriementSaveGameData();
			}
		}
	}

	_farmFPSSaveGame = Cast<UFarmFPSSaveGame>(UGameplayStatics::CreateSaveGameObject(UFarmFPSSaveGame::StaticClass()));
	_farmFPSSaveGame->SetPlayerSaveData(playerSaveData);
	_farmFPSSaveGame->SetUpgradeLocationSaveDatas(upgradeLocationSaveData);
	_farmFPSSaveGame->SetWeaponPickupSaveDatas(weaponPickupSaveData);
	_farmFPSSaveGame->SetTutorialSaveData(tutorialSaveData);
	_farmFPSSaveGame->SetBreadRequriementSaveData(breadRequirementSaveData);
	_farmFPSSaveGame->SetPlantSaveDatas(plantSaveDatas);
	
	UGameplayStatics::SaveGameToSlot(_farmFPSSaveGame, _playerSaveGameSlotName, 0);
	OnSavePlayerData.Broadcast();
}

void USaveGameManager::SaveGameSettings()
{
	FAudioManagerSaveGameData audioSaveData;
	if (ensure(IsValid(GetOwner())))
	{
		UAudioManager* audioManager = GetOwner()->FindComponentByClass<UAudioManager>();
		if (IsValid(audioManager))
		{
			audioSaveData = audioManager->GetSaveGameData();
		}
	}

	_farmFPSSaveGameSettings = Cast<UFarmFPSSaveGameSettings>(UGameplayStatics::CreateSaveGameObject(UFarmFPSSaveGameSettings::StaticClass()));
	_farmFPSSaveGameSettings->SetAudioManagerSaveData(audioSaveData);
	UGameplayStatics::SaveGameToSlot(_farmFPSSaveGameSettings, _playerSaveGameSettingsSlotName, 0);
	OnSavePlayerSettingsData.Broadcast();
}

void USaveGameManager::LoadGame()
{
	if (HasSaveGame())
	{
		if (!IsValid(_farmFPSSaveGame))
		{
			_farmFPSSaveGame = Cast<UFarmFPSSaveGame>(UGameplayStatics::LoadGameFromSlot(_playerSaveGameSlotName, 0));
		}
		OnLoadGameData.Broadcast(_farmFPSSaveGame);
	}
}

void USaveGameManager::LoadGameSettings()
{
	if (HasSaveGameSettings())
	{
		if (!IsValid(_farmFPSSaveGameSettings))
		{
			_farmFPSSaveGameSettings = Cast<UFarmFPSSaveGameSettings>(UGameplayStatics::LoadGameFromSlot(_playerSaveGameSettingsSlotName, 0));
		}
		OnLoadGameSettingsData.Broadcast(_farmFPSSaveGameSettings);
	}
}

void USaveGameManager::DeleteSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(_playerSaveGameSlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(_playerSaveGameSlotName, 0);
	}
}

void USaveGameManager::DeleteSaveGameSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(_playerSaveGameSettingsSlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(_playerSaveGameSettingsSlotName, 0);
	}
}

bool USaveGameManager::HasSaveGame() const
{
	return UGameplayStatics::DoesSaveGameExist(_playerSaveGameSlotName, 0);
}

bool USaveGameManager::HasSaveGameSettings() const
{
	return UGameplayStatics::DoesSaveGameExist(_playerSaveGameSettingsSlotName, 0);
}
