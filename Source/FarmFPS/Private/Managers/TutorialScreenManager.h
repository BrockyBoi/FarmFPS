// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/DayNightCycleManager.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "TutorialScreenManager.generated.h"

UENUM(BlueprintType)
enum class ETutorialScreenType : uint8
{
	PickUpLightGun,
	PickUpWaterGun,
	PickUpSeedGun,
	FullyGrowCrop,
	PickUpFirstResource,
	ThrowResourceIntoOven,
	ObtainFirstBread,
	GiveBreadToStand,
	FirstNight,
	MoonGoDown
};

enum class EDayState : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTutorialScreenManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTutorialScreenManager();

	bool HasShownTutorialScreen(ETutorialScreenType tutorialScreenType) const;
	bool GetShouldShowTutorials() const { return _shouldShowTutorials; }

	void PressCloseScreen();
	void PressStopShowingTutorials();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnWeaponCollected(const FGameplayTag& resourceType);

	UFUNCTION()
	void OnIngredientAddedToOven(const FGameplayTag& ingredientType);

	UFUNCTION()
	void OnBreadAddedToStand();

	UFUNCTION()
	void OnDayNightCycleStateChanged(EDayState newDayState);

	UFUNCTION()
	void OnResourceCollected(const FGameplayTag& resourceType);

	UFUNCTION()
	void OnPlantFullyGrown();

	void AttemptShowScreen(ETutorialScreenType screenToShow);

	TMap<ETutorialScreenType, bool> _shownTutorialScreensMap;
	bool _shouldShowTutorials = true;
};
