// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "PlayerWallet.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPlayerWallet : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerWallet();

	void AddMoney(uint16 moneyToAdd);
	void RemoveMoney(uint16 moneyToRemove);
	void SetMoney(uint16 newMoneyAmount);


	bool HasEnoughMoney(uint16 moneyToCheck) const { return _currentMoney >= moneyToCheck; }

	UFUNCTION(BlueprintPure)
	int GetCurrentMoneyCount() const { return _currentMoney; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	uint16 _currentMoney = 0;
};
