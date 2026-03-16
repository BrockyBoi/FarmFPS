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

void UCustomerQueue::RemoveCustomerFromFrontOfQueue()
{
	_customerQueue.RemoveAt(0);

	for (int i = 0; i < _customerQueue.Num(); i++)
	{
		ACustomer* customer = _customerQueue[i];
		if (IsValid(customer))
		{
			customer->MoveToNextSpotInQueue(GetQueuePositionAtIndex(i));
		}
	}
}

FVector UCustomerQueue::GetQueuePositionAtIndex(int index) const
{
	return ensure(IsValid(_queueStartPoint)) ? _queueStartPoint->GetComponentLocation() + (_queueDirection * index * _customerSpacing) : FVector();
}

FVector UCustomerQueue::GetEndQueuePosition() const
{
	return ensure(IsValid(_queueStartPoint)) ? _queueStartPoint->GetComponentLocation() + (_queueDirection * _customerQueue.Num() * _customerSpacing) : FVector();
}

bool UCustomerQueue::IsAtFrontOfQueue(const ACustomer* Customer) const
{
	return !_customerQueue.IsEmpty() && _customerQueue[0] == Customer;
}

ACustomer* UCustomerQueue::GetFrontOfQueue() const
{
	return !_customerQueue.IsEmpty() ? _customerQueue[0] : nullptr;
}
