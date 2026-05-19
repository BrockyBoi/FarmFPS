// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "BreadRequirementManager.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UBreadRequirementManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBreadRequirementManager();

	void SellBread(int breadAmount = 1);

	UFUNCTION(BlueprintPure)
	int GetCurrentBreadSold() const { return _currentBreadSold; }

	UFUNCTION(BlueprintPure)
	bool HasSoldBreadNeeded() const { return _currentBreadSold >= _startingBreadRequired.GetModifiedValue(this); }
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnDayBegin();

	UFUNCTION()
	void OnDayEnd();

	UFUNCTION()
	void OnBreadSold();

	void OnRequirementsMet();
	void OnDayFailed();

	int _currentBreadSold = 0;
	int _breadRequiredForCurrentDay = 0;

	bool _isFirstDay = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BreadRequirement", meta = (AllowPrivateAccess = true))
	FModifiedIntValue _startingBreadRequired = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BreadRequirement", meta = (AllowPrivateAccess = true))
	FModifiedFloatValue _dailyMultiplier = 1.5f;
};
