// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"
#include "ModifiedValueData.h"
#include "TradeOffData.h"
#include "TradeOffPossibility.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "TradeOffUpgradeManager.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTradeOffUpgradeManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTradeOffUpgradeManager();

protected:
	virtual void BeginPlay() override;

	void OnDayBegin();

private:
	void OnUpgradeAccepted();
	void OnUpgradeDeclined();

	void GenerateTradeOffs();

	FTradeOffData _currentTradeOff;

	UPROPERTY(EditDefaultsOnly)
	TMap<EPerkRarity, FPerkData> _breadIncreaseAmounts;

	UPROPERTY(EditDefaultsOnly)
	TMap<EPerkRarity, FModifiedFloatValue> _rarityChancePercentages;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _tradeOffPossiblitiesTable;

	TMap<FGameplayTag, FTradeOffPossibility> _possibilities;
};
