// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadStand.h"

// Brock
#include "Customer.h"
#include "CustomerQueue.h"
#include "ResourceInventory.h"

ABreadStand::ABreadStand() : Super()
{
	_customerQueue = CreateDefaultSubobject<UCustomerQueue>("CustomerQueue");
}

void ABreadStand::BeginPlay()
{
	Super::BeginPlay();
}

const FVector ABreadStand::GetNextCustomerQueuePosition()
{
	return ensure(IsValid(_customerQueue)) ? _customerQueue->GetEndQueuePosition() : FVector::ZeroVector;
}

const FModifiedResourceValue ABreadStand::GetPriceForResource(const FGameplayTag& resourceType) const
{
	for (const FModifiedResourceValue& priceData : _priceDataArray)
	{
		if (priceData.ResourceTag == resourceType)
		{
			return priceData;
		}
	}

	return FModifiedResourceValue();
}

void ABreadStand::OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, int amount)
{
	if (_isCurrentlySellingBreadToCustomer)
	{
		return;
	}

	if (ensure(IsValid(_customerQueue)))
	{
		ACustomer* customer = _customerQueue->GetFrontOfQueue();
		if (IsValid(customer))
		{
			customer->AttemptBuyBreadAtFrontOfQueue();
		}
	}
}
