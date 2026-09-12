// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "SaveGameManager.generated.h"

class UFarmFPSSaveGame;
class UFarmFPSSaveGameSettings;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USaveGameManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	USaveGameManager();

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void SaveGameSettings();

	UFUNCTION(BlueprintCallable)
	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void LoadGameSettings();

	UFUNCTION(BlueprintCallable)
	void DeleteSaveGame();

	UFUNCTION(BlueprintCallable)
	void DeleteSaveGameSettings();

	UFUNCTION(BlueprintPure)
	bool HasSaveGame() const;

	UFUNCTION(BlueprintPure)
	bool HasSaveGameSettings() const;

	DECLARE_MULTICAST_DELEGATE(FOnSavePlayerData);
	FOnSavePlayerData OnSavePlayerData;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadPlayerData, UFarmFPSSaveGame*);
	FOnLoadPlayerData OnLoadGameData;

	FOnSavePlayerData OnSavePlayerSettingsData;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadPlayerSettingsData, UFarmFPSSaveGameSettings*);
	FOnLoadPlayerSettingsData OnLoadGameSettingsData;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPostLoadMap(UWorld* world);

	void ResetSavableActors();

	UPROPERTY(EditAnywhere, Category = "Save Game")
	FString _playerSaveGameSlotName = TEXT("PlayerSaveGame");

	UPROPERTY(EditAnywhere, Category = "Save Game")
	FString _playerSaveGameSettingsSlotName = TEXT("PlayerSaveGameSettings");

	UPROPERTY()
	TObjectPtr<UFarmFPSSaveGame> _farmFPSSaveGame = nullptr;

	UPROPERTY()
	TObjectPtr<UFarmFPSSaveGameSettings> _farmFPSSaveGameSettings = nullptr;

	UPROPERTY(EditAnywhere, Category = "Save Game")
	TSubclassOf<AActor> _defaultUpgradePointBP;

	UPROPERTY(EditAnywhere, Category = "Save Game")
	TSubclassOf<AActor> _defaultUnlockActorPointBP;

	TArray<AActor*> _saveableActors;
};
