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

	const FPerkData GetPerkData(EPerkType PerkType) const;

	void ModifyAdditiveValue(EPerkType perkType, float valueChange);
	void ModifyMultiplicativeValue(EPerkType perkType, float valueToMultiplyBy);
	void ModifyPerkData(EPerkType perkType, const FPerkData& perkDataChange);

	float ModifyValueByPerks(EPerkType perkType, float valueToModify) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
	TMap<EPerkType, FPerkData> _activePerks;
};
