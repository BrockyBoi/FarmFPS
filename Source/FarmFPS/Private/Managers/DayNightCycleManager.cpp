// Fill out your copyright notice in the Description page of Project Settings.

#include "DayNightCycleManager.h"

// Brock
#include "TradeOffUpgradeManager.h"

// UE
#include "Components/AudioComponent.h"
#include "EngineUtils.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"

FStaticOnDayStateChange UDayNightCycleManager::OnDayStateChange;

UDayNightCycleManager::UDayNightCycleManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDayNightCycleManager::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		if (IsValid(actor))
		{
			if (!_sunLight.IsValid() && actor->Tags.Contains(TEXT("Sun")))
			{
				_sunLight = *ActorItr;
			}

			if (!_moonLight.IsValid() && actor->Tags.Contains(TEXT("Moon")))
			{
				_moonLight = *ActorItr;
			}
		}
	}

	StartDay();

	UTradeOffUpgradeManager* tradeOffUpgradeManager = UFarmFPSUtilities::GetTradeOffUpgradeManager(this);
	if (ensure(IsValid(tradeOffUpgradeManager)))
	{
		tradeOffUpgradeManager->OnTradeOffAnyInput.AddDynamic(this, &UDayNightCycleManager::StartDay);
	}
}

void UDayNightCycleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UTradeOffUpgradeManager* tradeOffUpgradeManager = UFarmFPSUtilities::GetTradeOffUpgradeManager(this);
	if (IsValid(tradeOffUpgradeManager))
	{
		tradeOffUpgradeManager->OnTradeOffAnyInput.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UDayNightCycleManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetCurrentDayState() == EDayState::Day && ensure(_sunLight.IsValid()))
	{
		_timeElapsed += DeltaTime;
		float dayLength = _dayLength.GetModifiedValue(this);
		float lerpedPitch = FMath::Lerp(0.f, _finalDayAngle, _timeElapsed / dayLength) + 180.f;

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_sunLight->SetActorRotation(rotation);

		if (_timeElapsed >= dayLength)
		{
			EndDay();
		}
	}

	if (GetCurrentDayState() == EDayState::MidNight && ensure(_moonLight.IsValid()))
	{
		if (_timeElapsed >= _timeToReachPeakMoon)
		{
			_moonLight->SetActorRotation(FRotator(250.f, 0.f, 0.f));
			return;
		}

		_timeElapsed += DeltaTime;
		float lerpedPitch = FMath::Lerp(0.f, 250.f, _timeElapsed / _timeToReachPeakMoon);

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_moonLight->SetActorRotation(rotation);

		if (_timeElapsed >= _timeToReachPeakMoon)
		{
			_musicAudioComponent->SetSound(_nightTimeMusic);
			_musicAudioComponent->FadeIn(0.5f, 1.f);
		}
	}

	if (GetCurrentDayState() == EDayState::NightTransitionToDay && ensure(_moonLight.IsValid()))
	{
		_timeElapsed += DeltaTime;
		float lerpedPitch = FMath::Lerp(70.f, 200.f, _timeElapsed / _timeToReachPeakMoon) + 180.f;

		FRotator rotation(lerpedPitch, 0.f, 0.f);
		_moonLight->SetActorRotation(rotation);

		if (_timeElapsed >= _timeToReachPeakMoon)
		{
			GenerateDailyTradeOff();
		}
	}
}

void UDayNightCycleManager::TransitionToNextDay()
{
	UGameplayStatics::SpawnSound2D(this, _onNightEndSound);
	if (ensure(IsValid(_musicAudioComponent)))
	{
		_musicAudioComponent->FadeOut(0.5f, 0.f);
	}
	_timeElapsed = 0.f;
	_currentDayState = EDayState::NightTransitionToDay;
}

float UDayNightCycleManager::GetPercentageDayElapsed() const
{
	if (_currentDayState == EDayState::Day)
	{
		return _timeElapsed / _dayLength.GetModifiedValue(this);
	}

	return 0.0f;
}

void UDayNightCycleManager::CHEAT_StartDay()
{
	if (GetCurrentDayState() == EDayState::MidNight)
	{
		TransitionToNextDay();
	}
}

void UDayNightCycleManager::CHEAT_StartNight()
{
	if (GetCurrentDayState() == EDayState::Day)
	{
		EndDay();
	}
}

void UDayNightCycleManager::StartDay()
{
	SetDayState(EDayState::Day);
	if (OnDayBegin.IsBound())
	{
		OnDayBegin.Broadcast();
	}

	if (ensure(_sunLight.IsValid()) && ensure(_moonLight.IsValid()))
	{
		_sunLight->SetActorHiddenInGame(false);
		_moonLight->SetActorHiddenInGame(true);

		FRotator rotation(90.f, 0.f, 0.f);
		_sunLight->SetActorRotation(rotation);
	}

	if (ensure(IsValid(_onDayStartSound)) && ensure(IsValid(_daytimeMusic)))
	{
		if (!IsValid(_musicAudioComponent))
		{
			_musicAudioComponent = UGameplayStatics::SpawnSound2D(this, _daytimeMusic);
			_musicAudioComponent->bAutoDestroy = false;
		}
		else
		{
			_musicAudioComponent->SetSound(_daytimeMusic);
			_musicAudioComponent->FadeIn(0.5f, 1.f);
			_musicAudioComponent->Play();
		}
	}

	UGameplayStatics::SpawnSound2D(this, _onDayStartSound);
}

void UDayNightCycleManager::EndDay()
{
	if (ensure(IsValid(_musicAudioComponent)))
	{
		_musicAudioComponent->FadeOut(2.f, 0.f);
	}

	SetDayState(EDayState::MidNight);
	if (OnDayEnd.IsBound())
	{
		OnDayEnd.Broadcast();
	}

	_timeElapsed = 0.f;

	if (ensure(_sunLight.IsValid()) && ensure(_moonLight.IsValid()))
	{
		_sunLight->SetActorHiddenInGame(true);
		_moonLight->SetActorHiddenInGame(false);

		FRotator moonRotation(90.f, 0.f, 0.f);
		_moonLight->SetActorRotation(moonRotation);
	}

	FTimerHandle timerHandle;
	//GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UDayNightCycleManager::StartDay, _timeUntilAutoStartNextDay, false);

	UGameplayStatics::SpawnSound2D(this, _onDayEndSound);
}

void UDayNightCycleManager::SetDayState(EDayState newDayState)
{
	_currentDayState = newDayState;
	OnDayStateChange.Broadcast(newDayState);
}

void UDayNightCycleManager::GenerateDailyTradeOff()
{
	SetDayState(EDayState::WaitingForTradeOff);
	if (ensure(_sunLight.IsValid()) && ensure(_moonLight.IsValid()))
	{
		_sunLight->SetActorHiddenInGame(false);
		_moonLight->SetActorHiddenInGame(true);

		FRotator rotation(90.f, 0.f, 0.f);
		_sunLight->SetActorRotation(rotation);
	}

	if (OnWaitingForTradeOff.IsBound())
	{
		OnWaitingForTradeOff.Broadcast();
	}
}

