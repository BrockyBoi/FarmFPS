// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/DayNightCycleManager.h"
#include "SaveSystem/Saveable.h"
#include "SaveSystem/TutorialSaveGameData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "TutorialScreenManager.generated.h"

UENUM(BlueprintType)
enum class ETutorialScreenType : uint8
{
	OnPlayerSpawned,
	PickUpLightGun,
	PickUpWaterGun,
	PickUpSeedGun,
	PlantFirstSeed,
	FillFirstCropWithLight,
	FullyGrowCrop,
	PickUpFirstResource,
	ThrowResourceIntoOven,
	FirstBreadSpawned,
	ObtainFirstBread,
	GiveBreadToStand,
	FirstNight,
	FirstUpgradePurchase,
	MoonGoDown
};

class ACrop;
class AResourcePickupActor;
class AShooterWeapon;
class ATutorialShineActor;
enum class EDayState : uint8;
enum class EShineSize : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllowTutorialScreenClose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialScreenForceClose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialScreenShown, ETutorialScreenType, tutorialScreenType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTutorialScreenManager : public UActorComponent, public ISaveable
{
	GENERATED_BODY()

public:	
	UTutorialScreenManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	bool CanCloseTutorialScreen() const;

	UFUNCTION(BlueprintPure, Category = "TutorialScreenManager")
	bool HasShownTutorialScreen(ETutorialScreenType tutorialScreenType) const;

	UFUNCTION(BlueprintPure, Category = "TutorialScreenManager")
	bool GetShouldShowTutorials() const { return _shouldShowTutorials; }

	UFUNCTION(BlueprintCallable, Category = "TutorialScreenManager")
	void PressCloseScreen();

	UFUNCTION(BlueprintCallable, Category = "TutorialScreenManager")
	void PressStopShowingTutorials();

	FTutorialSaveGameData GetTutorialSaveGameData() const;
	virtual void OnGameLoaded(UFarmFPSSaveGame* saveGame) override;

	bool HasPlayerCompletedBasicTutorial() const;

	UPROPERTY(BlueprintAssignable, Category = "TutorialScreenManager")
	FOnTutorialScreenShown OnTutorialScreenShown;

	UPROPERTY(BlueprintAssignable, Category = "TutorialScreenManager")
	FOnAllowTutorialScreenClose OnAllowTutorialScreenToClose;

	UPROPERTY(BlueprintAssignable, Category = "TutorialScreenManager")
	FOnTutorialScreenForceClose OnTutorialScreenForceClosed;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnWeaponCollected(AShooterWeapon* weapon);

	UFUNCTION()
	void OnStartBakingBread();

	UFUNCTION()
	void OnFirstBreadSpawned(AActor* breadActor);

	UFUNCTION()
	void OnBreadAddedToStand();

	UFUNCTION()
	void OnDayNightCycleStateChanged(EDayState newDayState);

	UFUNCTION()
	void OnResourceCollected(const FGameplayTag& resourceType);

	UFUNCTION()
	void OnCropPlanted(ACrop* crop);

	UFUNCTION()
	void OnPlantFullyGrown();

	UFUNCTION()
	void OnResourceSpawned(AResourcePickupActor* resourcePickupActor);

	UFUNCTION()
	void OnLevelReady();

	UFUNCTION()
	void OnFirstUpgradePurchased();

	bool AttemptShowScreen(ETutorialScreenType screenToShow);

	void HideTutorialShineObject();
	void TutorialShineObjectOnActor(AActor* actorToFollow, EShineSize shineSize);
	void TutorialShineObjectFollowActor(AActor* actorToFollow, EShineSize shineSize);

	UFUNCTION()
	void AllowTutorialScreenToClose();

	void OnFirstCropResourceFilled(const FGameplayTag& resource);

	TMap<ETutorialScreenType, bool> _shownTutorialScreensMap;
	bool _shouldShowTutorials = true;
	bool _allowTutorialScreenClose = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATutorialShineActor> _tutorialShineObjectClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> _wheatPlotBPClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> _weaponPickupBPClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> _upgradePointBPClass;

	TWeakObjectPtr<ATutorialShineActor> _tutorialShineObject = nullptr;
	TWeakObjectPtr<AActor> _waterPistol = nullptr;
	TWeakObjectPtr<AActor> _wheatPistol = nullptr;
	TWeakObjectPtr<ACrop> _firstCrop = nullptr;

	UPROPERTY(EditAnywhere)
	float _timeBeforeTutorialCanBeClosed = 1.5f;

	float _timeElapsedSinceTutorialOpen = 0.f;

	bool _hasFirstWheatSpawned = false;
	bool _hasFirstBreadSpawned = false;
};
