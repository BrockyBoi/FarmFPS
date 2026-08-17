// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadStand.h"

// Brock
#include "Characters/Customer.h"
#include "Managers/BreadRequirementManager.h"
#include "Managers/CustomerQueue.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Resources/ResourceTypeTags.h"

ABreadStand::ABreadStand() : Super()
{
	_customerQueue = CreateDefaultSubobject<UCustomerQueue>("CustomerQueue");
}

void ABreadStand::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayBegin.AddUObject(this, &ABreadStand::OnDayBegin);
		dayNightCycle->OnDayEnd.AddUObject(this, &ABreadStand::OnDayEnd);
	}
}

void ABreadStand::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayBegin.RemoveAll(this);
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
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

void ABreadStand::OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount)
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

void ABreadStand::OnDayBegin()
{
}

void ABreadStand::OnDayEnd()
{
	UBreadRequirementManager* breadRequirementManager = UFarmFPSUtilities::GetBreadRequirementManager(this);
	if (ensure(IsValid(breadRequirementManager)) && breadRequirementManager->GetHasSoldBreadRequiredForDay())
	{
		GetOutputInventory()->MultiplyResource(ResourceTypeTags::Money, _bonusPriceMultiplierOnAllBreadSold.GetModifiedValue(this));
	}
}
