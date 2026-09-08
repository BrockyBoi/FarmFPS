// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/SaveGameManager.h"

// FarmFPS
#include "FarmFPSCharacter.h"
#include "ShooterPickup.h"

// Brock
#include "FarmFPSSaveGame.h"
#include "Interactables/PurchaseLocation.h"
#include "Managers/BreadRequirementManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/TutorialScreenManager.h"

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
	if (ensure(IsValid(dayNightCycle)))
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

			UTutorialScreenManager* tutorialScreenManager = Cast<UTutorialScreenManager>(saveableActor->FindComponentByClass(UTutorialScreenManager::StaticClass()));
			if (IsValid(tutorialScreenManager))
			{
				tutorialSaveData = tutorialScreenManager->GetTutorialSaveGameData();
			}

			UBreadRequirementManager* breadRequirementManager = Cast<UBreadRequirementManager>(saveableActor->FindComponentByClass(UBreadRequirementManager::StaticClass()));
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

	UGameplayStatics::SaveGameToSlot(_farmFPSSaveGame, _playerSaveGameSlotName, 0);
	OnSavePlayerData.Broadcast();
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

void USaveGameManager::DeleteSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(_playerSaveGameSlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(_playerSaveGameSlotName, 0);
	}
}

bool USaveGameManager::HasSaveGame() const
{
	return UGameplayStatics::DoesSaveGameExist(_playerSaveGameSlotName, 0);
}
