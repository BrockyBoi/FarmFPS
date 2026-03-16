// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "AIController.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"

// Generated
#include "Customer.generated.h"

class ABreadStand;
class UCustomerQueue;

UCLASS()
class ACustomer : public ACharacter
{
	GENERATED_BODY()

public:
	ACustomer();

	void AttemptBuyBreadAtFrontOfQueue();

	void FindSpotInQueue();
	void MoveToNextSpotInQueue(const FVector& nextSpot);
	void MoveOutOfMap();
	virtual void Tick(float DeltaTime) override;

	const FGameplayTag& GetResourceDesired() const { return _resourceDesired; }
	const int GetAmountDesired() const { return _amountDesired; }
	const int GetAmountLeftToBuy() const { return _amountLeftToBuy; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void OnMoveFinishedOutOfMap(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void OnMoveFinishedInQueue(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."))
	FGameplayTag _resourceDesired;

	int _amountDesired = 0;
	int _amountLeftToBuy = 0;

	UPROPERTY(EditDefaultsOnly)
	int _minCanDesire = 1;

	UPROPERTY(EditDefaultsOnly)
	int _maxCanDesire = 5;

	UPROPERTY(EditDefaultsOnly)
	float _moveAcceptanceRadius = 5.f;

	FVector _startLocation = FVector::ZeroVector;
	FVector _nextDestination = FVector::ZeroVector;

	TWeakObjectPtr<ABreadStand> _breadStand;
	TWeakObjectPtr<UCustomerQueue> _customerQueue;

	UPROPERTY(Transient)
	AAIController* _aiController = nullptr;

	bool _reachedSpotInQueue = false;
};
