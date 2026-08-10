// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"

// Generated
#include "BreadRequirementManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMFPS_API UBreadRequirementManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBreadRequirementManager();

	void SellBread(int breadAmount = 1);

	UFUNCTION(BlueprintPure)
	int GetCurrentBreadSold() const { return _currentBreadSold; }

	UFUNCTION(BlueprintPure)
	bool HasSoldBreadNeeded() const { return _currentBreadSold >= _breadRequiredForCurrentDay; }

	UFUNCTION(BlueprintPure)
	int GetBreadRequiredForDay() const { return _breadRequiredForCurrentDay; }

	UFUNCTION(BlueprintPure)
	bool GetHasSoldBreadRequiredForDay() const { return _currentBreadSold >= _breadRequiredForCurrentDay; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBreadSold, int, BreadSold);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequirementsMet);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayFailed);

	UPROPERTY(BlueprintAssignable)
	FOnBreadSold OnBreadSold;

	UPROPERTY(BlueprintAssignable)
	FOnRequirementsMet OnRequirementsMet;

	UPROPERTY(BlueprintAssignable)
	FOnDayFailed OnDayFailed;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BreadRequirement", meta = (AllowPrivateAccess = true))
	int _startingBreadRequired = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BreadRequirement", meta = (AllowPrivateAccess = true))
	FModifiedIntValue _dailyBreadIncreaseAmount = 1;

private:
	UFUNCTION()
	void OnDayBegin();

	UFUNCTION()
	void OnDayEnd();

	void RequirementsMet();
	void DayFailed();

	int _currentBreadSold = 0;
	int _breadRequiredForCurrentDay = 0;

	bool _isFirstDay = true;
};
