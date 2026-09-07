// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "BreadRequirementManagerSaveGameData.h"
#include "PlayerSaveData.h"
#include "TutorialSaveGameData.h"
#include "UpgradeLocationSaveData.h"
#include "WeaponPickupSaveGameData.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

// Generated
#include "FarmFPSSaveGame.generated.h"

UCLASS()
class UFarmFPSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	const FPlayerSaveData& GetPlayerSaveData() { return PlayerSaveData; }
	const TArray<FUpgradeLocationSaveData>& GetUpgradeLocationSaveDatas() { return UpgradeLocationSaveDatas; }
	const TArray<FWeaponPickupSaveGameData>& GetWeaponPickupSaveDatas() { return WeaponPickupSaveDatas; }
	const FTutorialSaveGameData& GetTutorialSaveData() { return TutorialSaveData; }
	const FBreadRequirementManagerSaveGameData& GetBreadRequirmentSaveData() { return BreadRequirementSaveData; }

	void SetPlayerSaveData(const FPlayerSaveData& newPlayerSaveData) { PlayerSaveData = newPlayerSaveData; }
	void SetUpgradeLocationSaveDatas(const TArray<FUpgradeLocationSaveData>& newUpgradeLocationSaveDatas) { UpgradeLocationSaveDatas = newUpgradeLocationSaveDatas; }
	void SetWeaponPickupSaveDatas(const TArray<FWeaponPickupSaveGameData>& newWeaponPickupSaveDatas) { WeaponPickupSaveDatas = newWeaponPickupSaveDatas; }
	void SetTutorialSaveData(const FTutorialSaveGameData& tutorialSaveData) { TutorialSaveData = tutorialSaveData; }
	void SetBreadRequriementSaveData(const FBreadRequirementManagerSaveGameData& breadRequirementSaveData) { BreadRequirementSaveData = breadRequirementSaveData; }

protected:
	UPROPERTY()
	FPlayerSaveData PlayerSaveData;

	UPROPERTY()
	TArray<FUpgradeLocationSaveData> UpgradeLocationSaveDatas;

	UPROPERTY()
	TArray<FWeaponPickupSaveGameData> WeaponPickupSaveDatas;

	UPROPERTY()
	FTutorialSaveGameData TutorialSaveData;

	UPROPERTY()
	FBreadRequirementManagerSaveGameData BreadRequirementSaveData;
};
