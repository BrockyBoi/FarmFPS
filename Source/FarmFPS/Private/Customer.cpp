// Fill out your copyright notice in the Description page of Project Settings.

#include "Customer.h"

// Brock
#include "BreadStand.h"
#include "CustomerQueue.h"
#include "CustomerSpawnerManager.h"
#include "FarmFPSUtilities.h"
#include "ObjectiveManager.h"
#include "ObjectiveTypeTags.h"
#include "ResourceInventory.h"
#include "ResourceTypeTags.h"

// UE
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

ACustomer::ACustomer()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACustomer::BeginPlay()
{
	Super::BeginPlay();
	
	int modifiedMin = _minCanDesire.GetModifiedValue(this);
	int modifiedMax = _maxCanDesire.GetModifiedValue(this);
	_amountDesired = FMath::RandRange(modifiedMin, modifiedMax);
	_amountLeftToBuy = _amountDesired;

	_startLocation = GetActorLocation();

	_breadStand = FarmFPSUtilities::GetBreadStand(this);
	if (ensure(_breadStand.IsValid()) && ensure(IsValid(_breadStand->GetCustomerQueue())))
	{
		_customerQueue = _breadStand->GetCustomerQueue();
		MoveToBreadStand();
	}

	UCharacterMovementComponent* movement = FindComponentByClass<UCharacterMovementComponent>();
	if (ensure(IsValid(movement)))
	{
		movement->MaxWalkSpeed = _customerMoveSpeed.GetModifiedValue(this);
	}
}

void ACustomer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* aiController = Cast<AAIController>(NewController);
	if (ensure(IsValid(aiController)))
	{
		_aiController = aiController;
		_aiController->ReceiveMoveCompleted.AddDynamic(this, &ACustomer::OnMoveFinishedMovingToBreadStand);
	}
}

void ACustomer::OnMoveFinishedMovingToBreadStand(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		if (ensure(_customerQueue.IsValid()))
		{
			_customerQueue->AddCustomerToQueue(this);
			_aiController->ReceiveMoveCompleted.RemoveAll(this);
			_aiController->ReceiveMoveCompleted.AddDynamic(this, &ACustomer::OnMoveFinishedInQueue);
			MoveToNextSpotInQueue(_customerQueue->GetCustomerQueuePosition(this));
		}
	}
	else if (Result == EPathFollowingResult::Blocked)
	{
		FTimerHandle timerDel;
		GetWorld()->GetTimerManager().SetTimer(timerDel, this, &ACustomer::MoveToBreadStand, 1.f, false);
	}
}

void ACustomer::OnMoveFinishedOutOfMap(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		UCustomerSpawnerManager* spawnerManager = FarmFPSUtilities::GetCustomerSpawnerManager(this);
		if (ensure(IsValid(spawnerManager)))
		{
			spawnerManager->OnCustomerLeaveMap();
		}

		Destroy();
	}
}

void ACustomer::OnMoveFinishedInQueue(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		_reachedSpotInQueue = true;
		AttemptBuyBreadAtFrontOfQueue();
	}
	else if (Result == EPathFollowingResult::Blocked)
	{
		FTimerHandle timerDel;
		GetWorld()->GetTimerManager().SetTimer(timerDel, this, &ACustomer::FindSpotInQueue, 1.f, false);
	}
}

void ACustomer::AttemptBuyBreadAtFrontOfQueue()
{
	if (!_reachedSpotInQueue)
	{
		return;
	}

	if (!ensure(_breadStand.IsValid()) || !ensure(_customerQueue.IsValid()) || !_customerQueue->IsAtFrontOfQueue(this) || _breadStand->GetIsCurrentlySellingBreadToCustomer())
	{
		return;
	}

	UResourceInventory* breadInventory = _breadStand->GetInputInventory();
	if (ensure(IsValid(breadInventory)))
	{
		const int amountCanBuy = FMath::Min(_amountDesired, breadInventory->GetResourceCount(GetResourceDesired()));
		if (amountCanBuy > 0)
		{
			_breadStand->SetIsCurrentlySellingBreadToCustomer(true);

			breadInventory->RemoveResource(GetResourceDesired(), amountCanBuy);
			_amountLeftToBuy -= amountCanBuy;
			_breadStand->SetIsCurrentlySellingBreadToCustomer(false);

			if (_amountLeftToBuy <= 0)
			{
				_reachedSpotInQueue = false;
				const FModifiedResourceValue priceData = _breadStand->GetPriceForResource(GetResourceDesired());
				const int price = priceData.ModifiedIntValue.GetModifiedValue(this);

				_breadStand->GetOutputInventory()->AddResource(ResourceTypeTags::Money, GetAmountDesired() * price);
				UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
				if (ensure(IsValid(objectiveManager)))
				{
					objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::SellBread, GetResourceDesired(), GetAmountDesired());
				}

				_customerQueue->RemoveCustomerFromFrontOfQueue();

				_aiController->ReceiveMoveCompleted.RemoveAll(this);
				MoveOutOfMap();
			}
		}
	}
}

void ACustomer::FindSpotInQueue()
{
	MoveToNextSpotInQueue(_customerQueue->GetEndQueuePosition());
}

void ACustomer::MoveToBreadStand()
{
	_nextDestination = _breadStand->GetActorLocation();
	_reachedSpotInQueue = false;
	if (ensure(IsValid(_aiController)))
	{
		_aiController->MoveToLocation(_nextDestination, _moveAcceptanceRadius * 10, true, true, true);
	}
}

void ACustomer::MoveToNextSpotInQueue(const FVector& nextSpot)
{
	_nextDestination = nextSpot;
	_reachedSpotInQueue = false;
	if (ensure(IsValid(_aiController)))
	{
		_aiController->MoveToLocation(_nextDestination, _moveAcceptanceRadius, true, true, true);
	}
}

void ACustomer::MoveOutOfMap()
{
	if (ensure(IsValid(_aiController)))
	{
		_aiController->ReceiveMoveCompleted.AddDynamic(this, &ACustomer::OnMoveFinishedOutOfMap);
		_aiController->MoveToLocation(_startLocation, _moveAcceptanceRadius, true, true, true);
	}
}