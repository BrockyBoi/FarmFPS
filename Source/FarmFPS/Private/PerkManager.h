// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "PerkManager.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPerkManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPerkManager();

	const FPerkData GetPerkData(const FGameplayTag& perkTag) const;

	void ModifyAdditiveValue(const FGameplayTag& perkTag, float valueChange);
	void ModifyMultiplicativeValue(const FGameplayTag& perkTag, float valueToMultiplyBy);
	void ModifyPerkData(const FGameplayTag& perkTag, const FPerkData& perkDataChange);

	float ModifyValueByPerks(const FGameplayTag& perkTag, float valueToModify) const;
	float ModifyValueByPerks(const FGameplayTagContainer& perkTags, float valueToModify) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
	TMap<FGameplayTag, FPerkData> _activePerks;
};
