// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "PerkManager.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPerkManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPerkManager();

	const FPerkData GetPerkData(EPerkModifiers PerkType) const;

	void ModifyAdditiveValue(EPerkModifiers perkType, float valueChange);
	void ModifyMultiplicativeValue(EPerkModifiers perkType, float valueToMultiplyBy);
	void ModifyPerkData(EPerkModifiers perkType, const FPerkData& perkDataChange);

	float ModifyValueByPerks(EPerkModifiers perkType, float valueToModify) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
	TMap<EPerkModifiers, FPerkData> _activePerks;
};
