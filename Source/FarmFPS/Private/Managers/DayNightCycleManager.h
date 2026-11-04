// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Data/ModifiedValueData.h"
#include "Misc/CachedModifiableValueDataTableRowHandle.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "DayNightCycleManager.generated.h"

UENUM()
enum class EDayState : uint8
{
	WaitingForTradeOff,
	Day,
	MidNight,
	NightTransitionToDay
};

class ADirectionalLight;
class UAudioManager;
class UFarmFPSSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBPOnDayStateChange);
DECLARE_MULTICAST_DELEGATE_OneParam(FStaticOnDayStateChange, EDayState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UDayNightCycleManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDayNightCycleManager();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ForceEndDay();
	void ForceSetTimeLeft(float secondsLeftInDay);

	float GetModifiedDayLength() const;

	UFUNCTION(BlueprintPure)
	bool IsDay() const { return _currentDayState == EDayState::Day; }

	UFUNCTION(BlueprintPure)
	bool IsNight() const { return _currentDayState == EDayState::MidNight || _currentDayState == EDayState::NightTransitionToDay; }

	UFUNCTION(BlueprintPure)
	EDayState GetCurrentDayState() const { return _currentDayState; }

	EDayState GetResetDayState() const { return _dayStateOnDayReset; }

	void TransitionToNextDay();

	UFUNCTION(BlueprintPure)
	float GetPercentageDayElapsed() const;

	UFUNCTION(BlueprintPure)
	FLinearColor GetDayProgressBarColor() const;

	void CHEAT_StartDay();
	void CHEAT_StartNight();
	void CHEAT_UnlimitedDay(bool isUnlimited);

	DECLARE_MULTICAST_DELEGATE(FOnDayStateChange);

	FOnDayStateChange OnDayBegin;
	FOnDayStateChange OnDayEnd;
	FOnDayStateChange OnWaitingForTradeOff;

	UPROPERTY(BlueprintAssignable)
	FBPOnDayStateChange BPOnDayBegin;

	UPROPERTY(BlueprintAssignable)
	FBPOnDayStateChange BPOnDayEnd;

	UPROPERTY(BlueprintAssignable)
	FBPOnDayStateChange BPOnWaitingForTradeOff;

	static FStaticOnDayStateChange OnDayStateChange;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void StartDay();
	void EndDay();

	TObjectPtr<UAudioComponent> GetMusicAudioComponent(TSoftObjectPtr<USoundBase> musicClip);

	UFUNCTION()
	void OnPlayerPerkValueChange(const FGameplayTag& resourceType, const FPerkData& perkData);

	void EndDayFromLoading(UFarmFPSSaveGame*);

	void SetDayState(EDayState newDayState);

	void GenerateDailyTradeOff();

	void OnCustomerServed();

	void SetNightSkyOpacityPercentage(float percent);

	TWeakObjectPtr<UAudioManager> _audioManager;

	UPROPERTY(EditDefaultsOnly, Category = "Tunables")
	FCachedModifiableValueDataTableRowHandle _dayLength = 60.f;

	float _modifiedDayLength = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Tunables")
	FCachedModifiableValueDataTableRowHandle _timeToRemoveOnCustomerServed = 5.f;

	UPROPERTY(EditDefaultsOnly)
	float _timeToReachPeakMoon = 2.f;

	UPROPERTY(EditDefaultsOnly)
	float _timeUntilAutoStartNextDay = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float _finalDayAngle = 220.f;

	UPROPERTY(EditDefaultsOnly)
	TWeakObjectPtr<ADirectionalLight> _sunLight = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TWeakObjectPtr<ADirectionalLight> _moonLight = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> _onDayStartSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> _onDayEndSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> _daytimeMusic = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> _nightTimeMusic = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> _onNightEndSound = nullptr;

	TWeakObjectPtr<AActor> _nightSkyActor;

	UPROPERTY(EditAnywhere)
	FLinearColor _startOfDayColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere)
	FLinearColor _midDayColor = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere)
	FLinearColor _warningColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere)
	FLinearColor _endOfDayColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere)
	float _percentageToShowWarningColors = .85f;

	UPROPERTY(EditAnywhere)
	float _sinWaveMultiplier = 5;

	float _timeElapsed = 0.f;
	float _adjustedTimeElapsed = 0.f;

	UPROPERTY(EditAnywhere)
	float _percentageOfDayWhenNightSkyIsGone = .1f;

	UPROPERTY(EditAnywhere)
	float _percentageOfDayWhenNightSkyShows = .9f;

	UPROPERTY(EditAnywhere)
	EDayState _dayStateOnDayReset = EDayState::MidNight;

	EDayState _currentDayState = EDayState::Day;

	bool _CHEATIsUnlimitedDay = false;
};
