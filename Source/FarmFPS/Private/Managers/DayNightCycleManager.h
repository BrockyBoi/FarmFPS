// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "DayNightCycleManager.generated.h"

enum class EDayState : uint8
{
	Day,
	MidNight,
	NightTransitionToDay
};

class ADirectionalLight;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UDayNightCycleManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDayNightCycleManager();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsDay() const { return _currentDayState == EDayState::Day; }
	bool IsNight() const { return _currentDayState == EDayState::MidNight || _currentDayState == EDayState::NightTransitionToDay; }
	EDayState GetCurrentDayState() const { return _currentDayState; }

	void TransitionToNextDay();

	DECLARE_MULTICAST_DELEGATE(FOnDayChange);

	FOnDayChange OnDayBegin;
	FOnDayChange OnDayEnd;
protected:
	virtual void BeginPlay() override;

private:
	void StartDay();
	void EndDay();

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _dayLength = 60.f;

	UPROPERTY(EditDefaultsOnly)
	float _timeToReachPeakMoon = 2.f;

	UPROPERTY(EditDefaultsOnly)
	float _timeUntilAutoStartNextDay = 10.f;

	UPROPERTY(EditDefaultsOnly)
	TWeakObjectPtr<ADirectionalLight> _sunLight = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TWeakObjectPtr<ADirectionalLight> _moonLight = nullptr;

	TObjectPtr<UAudioComponent> _musicAudioComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onDayStartSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onDayEndSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _daytimeMusic = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _nightTimeMusic = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onNightEndSound = nullptr;

	float _timeElapsed = 0.f;

	EDayState _currentDayState;
};
