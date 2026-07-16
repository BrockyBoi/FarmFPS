// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"

 // Generated
#include "OnCropHitEffect.generated.h"

class ACrop;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FARMFPS_API UOnCropHitEffect : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetResourceType(FGameplayTag resourceType);
	virtual void TickEffect(float deltaTime);

protected:
	virtual void OnHitCrop(ACrop* crop);
	virtual void OnEffectStarted();
	virtual void StopEffect();

	FModifiedFloatValue _procChanceOnHit = .15f;
	TWeakObjectPtr<ACrop> _cropToAffect = nullptr;
	FGameplayTag _resourceType;
};
