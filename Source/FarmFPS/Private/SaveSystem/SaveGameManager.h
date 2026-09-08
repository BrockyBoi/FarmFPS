// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "SaveGameManager.generated.h"

class UFarmFPSSaveGame;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USaveGameManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	USaveGameManager();

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void DeleteSaveGame();

	UFUNCTION(BlueprintPure)
	bool HasSaveGame() const;

	DECLARE_MULTICAST_DELEGATE(FOnSavePlayerData);
	FOnSavePlayerData OnSavePlayerData;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadPlayerData, UFarmFPSSaveGame*);
	FOnLoadPlayerData OnLoadGameData;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPostLoadMap(UWorld* world);

	void ResetSavableActors();

	UPROPERTY(EditAnywhere, Category = "Save Game")
	FString _playerSaveGameSlotName = TEXT("PlayerSaveGame");

	UPROPERTY()
	TObjectPtr<UFarmFPSSaveGame> _farmFPSSaveGame = nullptr;

	UPROPERTY(EditAnywhere, Category = "Save Game")
	TSubclassOf<AActor> _defaultUpgradePointBP;

	UPROPERTY(EditAnywhere, Category = "Save Game")
	TSubclassOf<AActor> _defaultUnlockActorPointBP;

	TArray<AActor*> _saveableActors;
};
