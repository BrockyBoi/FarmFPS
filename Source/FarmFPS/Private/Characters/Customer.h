// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "AIController.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"

// Generated
#include "Customer.generated.h"

class ABreadStand;
class UCustomerQueue;

enum class ECustomerState
{
	MovingToBreadStand,
	MovingToSpotInQueue,
	InQueue,
	MovingOutOfMap
};

UCLASS()
class ACustomer : public ACharacter
{
	GENERATED_BODY()

public:
	ACustomer();

	void AttemptBuyBreadAtFrontOfQueue();

	void FindSpotInQueue();
	void MoveToBreadStand();
	void MoveToNextSpotInQueue(const FVector& nextSpot);
	void MoveOutOfMap();

	UFUNCTION(BlueprintPure)
	const FGameplayTagContainer& GetResourcesDesired() const { return _resourcesDesired; }

	UFUNCTION(BlueprintPure)
	const int GetAmountDesired() const { return _amountDesired; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void OnMoveFinishedMovingToBreadStand(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	UFUNCTION()
	void OnMoveFinishedOutOfMap(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void OnMoveFinishedInQueue(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void OnDayEnd();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_SetBreadDesiredUI();

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."))
	FGameplayTagContainer _resourcesDesired;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _bonusMoneyValue = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onBoughtBreadSound;

	int _amountDesired = 0;

	UPROPERTY(EditDefaultsOnly)
	FModifiedIntValue _minCanDesire;

	UPROPERTY(EditDefaultsOnly)
	FModifiedIntValue _maxCanDesire;

	UPROPERTY(EditDefaultsOnly)
	float _moveAcceptanceRadius = 5.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _customerMoveSpeed = 600;

	FVector _startLocation = FVector::ZeroVector;
	FVector _nextDestination = FVector::ZeroVector;

	TWeakObjectPtr<ABreadStand> _breadStand;
	TWeakObjectPtr<UCustomerQueue> _customerQueue;

	UPROPERTY(Transient)
	AAIController* _aiController = nullptr;

	ECustomerState _currentState = ECustomerState::MovingToBreadStand;
};
