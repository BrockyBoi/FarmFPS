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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialScreenShown, ETutorialScreenType, tutorialScreenType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTutorialScreenManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTutorialScreenManager();

	UFUNCTION(BlueprintPure, Category = "TutorialScreenManager")
	bool HasShownTutorialScreen(ETutorialScreenType tutorialScreenType) const;

	UFUNCTION(BlueprintPure, Category = "TutorialScreenManager")
	bool GetShouldShowTutorials() const { return _shouldShowTutorials; }

	UFUNCTION(BlueprintCallable, Category = "TutorialScreenManager")
	void PressCloseScreen();

	UFUNCTION(BlueprintCallable, Category = "TutorialScreenManager")
	void PressStopShowingTutorials();

	UPROPERTY(BlueprintAssignable, Category = "TutorialScreenManager")
	FOnTutorialScreenShown OnTutorialScreenShown;

protected:
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
