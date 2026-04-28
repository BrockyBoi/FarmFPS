// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomerQueue.h"

// Brock
#include "Customer.h"

UCustomerQueue::UCustomerQueue()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCustomerQueue::BeginPlay()
{
	Super::BeginPlay();
}

void UCustomerQueue::AddCustomerToQueue(ACustomer* Customer)
{
	_customerQueue.Add(Customer);
}

void UCustomerQueue::RemoveCustomerFromQueue(ACustomer* Customer)
{
	_customerQueue.Remove(Customer);
}

void UCustomerQueue::RemoveCustomerFromFrontOfQueue()
{
	_customerQueue.RemoveAt(0);

	for (int i = 0; i < _customerQueue.Num(); i++)
	{
		TWeakObjectPtr<ACustomer> customer = _customerQueue[i];
		if (customer.IsValid())
		{
			customer->MoveToNextSpotInQueue(GetQueuePositionAtIndex(i));
		}
	}
}

const FVector UCustomerQueue::GetCustomerQueuePosition(const ACustomer* searchingCustomer) const
{
	if (_customerQueue.Contains(searchingCustomer))
	{
		int32 index = _customerQueue.IndexOfByPredicate([&, searchingCustomer](const TWeakObjectPtr<ACustomer> customer)
			{
				return customer.IsValid() && customer == searchingCustomer;
			});

		return GetQueuePositionAtIndex(index);
	}
	return FVector::ZeroVector;
}

const FVector UCustomerQueue::GetQueuePositionAtIndex(int index) const
{
	return ensure(IsValid(_queueStartPoint)) ? _queueStartPoint->GetComponentLocation() + (_queueDirection * index * _customerSpacing) : FVector::ZeroVector;
}

const FVector UCustomerQueue::GetEndQueuePosition() const
{
	return ensure(IsValid(_queueStartPoint)) ? _queueStartPoint->GetComponentLocation() + (_queueDirection * _customerQueue.Num() * _customerSpacing) : FVector::ZeroVector;
}

bool UCustomerQueue::IsAtFrontOfQueue(const ACustomer* Customer) const
{
	return !_customerQueue.IsEmpty() && _customerQueue[0] == Customer;
}

ACustomer* UCustomerQueue::GetFrontOfQueue() const
{
	return !_customerQueue.IsEmpty() ? _customerQueue[0].Get() : nullptr;
}
