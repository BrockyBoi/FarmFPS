// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "StatusEffect.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "EffectManager.generated.h"

USTRUCT(BlueprintType)
struct FAddResourceOverTimeEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue PercentageToProcEffect;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue LingerDuration;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue ResourcesPerSecond;
};

USTRUCT(BlueprintType)
struct FArcBetweenCropsEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue PercentageToProcEffect;

	UPROPERTY(EditDefaultsOnly)
	FModifiedIntValue MaxArcCount;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue ResourcePercentageOnArc;
};

enum class EStatusEffectType
{
	Arc,
	Lingering
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEffectManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEffectManager();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TriggerEffect(EStatusEffectType effectType);

	void AddLingeringEffect(StatusEffect& statusEffect);
	void RemoveLingeringEffect(StatusEffect& statusEffect);

protected:
	virtual void BeginPlay() override;

	TArray<StatusEffect> _lingeringStatusEffects = TArray<StatusEffect>();
};
