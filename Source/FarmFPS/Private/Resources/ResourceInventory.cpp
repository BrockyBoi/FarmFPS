// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceInventory.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Resources/ResourceTypeTags.h"

UResourceInventory::UResourceInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceInventory::BeginPlay()
{
	Super::BeginPlay();

	ListenToDayCycleEvents(true);
}

void UResourceInventory::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ListenToDayCycleEvents(false);
	_resourcesMap.Empty();
	Super::EndPlay(EndPlayReason);
}

void UResourceInventory::AddResource(const FGameplayTag& resourceType, float amount)
{
	if (_canAddResources && amount > 0)
	{
		SetResourceAmount(resourceType, GetResourceCount(resourceType) + amount);
	}
}

void UResourceInventory::SetResourceCap(const FGameplayTag& resourceType, float cap)
{
	if (cap > 0)
	{
		_resourceCaps.FindOrAdd(resourceType, cap) = cap;
	}
}

void UResourceInventory::RemoveResource(const FGameplayTag& resourceType, float amount)
{
	if (amount > 0 && ensure(HasResourceAmount(resourceType, amount)))
	{
		SetResourceAmount(resourceType, GetResourceCount(resourceType) - amount);
	}
}

void UResourceInventory::SetResourceAmount(const FGameplayTag& resourceType, float newAmount)
{
	if (FMath::IsNearlyEqual(GetResourceCount(resourceType), newAmount))
	{
		return;
	}
	
	float& resourceCount = _resourcesMap.FindOrAdd(resourceType, 0);
	resourceCount = FMath::Clamp(newAmount, 0, GetResourceCap(resourceType));

	OnResourceCountChanged.Broadcast(resourceType, GetResourceCount(resourceType));
}

void UResourceInventory::AddAllResourcesInInventory(UResourceInventory* otherInventory)
{
	if (_canAddResources && ensure(IsValid(otherInventory)))
	{
		for (auto pair : otherInventory->_resourcesMap)
		{
			AddResource(pair.Key, pair.Value);
			otherInventory->RemoveResource(pair.Key, pair.Value);
		}
	}
}

bool UResourceInventory::CanAddResource(const FGameplayTag& resourceType) const
{
	return _canAddResources && !IsResourceFull(resourceType);
}

void UResourceInventory::ListenToDayCycleEvents(bool listen)
{
	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);

	if (IsValid(dayNightCycle))
	{
		if (listen)
		{
			dayNightCycle->OnDayBegin.AddUObject(this, &UResourceInventory::OnDayBegin);
			dayNightCycle->OnDayEnd.AddUObject(this, &UResourceInventory::OnDayEnd);
		}
		else
		{
			dayNightCycle->OnDayBegin.RemoveAll(this);
			dayNightCycle->OnDayEnd.RemoveAll(this);
		}
	}
}

float UResourceInventory::GetResourceCount(const FGameplayTag& resourceType) const
{
	return _resourcesMap.FindOrAdd(resourceType, 0.f);
}

bool UResourceInventory::HasResourceAmount(const FGameplayTag& resourceType, float amount) const
{
	float currentAmount = GetResourceCount(resourceType);
	return FMath::IsNearlyEqual(currentAmount, amount) || currentAmount > amount;
}

uint16 UResourceInventory::GetResourceCap(const FGameplayTag& resourceType) const
{
	const float resourceCap = _resourceCaps.FindOrAdd(resourceType, 0.f);
	return resourceCap > 0 ? resourceCap : _defaultResourceCap;
}

bool UResourceInventory::IsResourceFull(const FGameplayTag& resourceType) const
{
	const float resourceCount = GetResourceCount(resourceType);
	const float cap = GetResourceCap(resourceType);

	return FMath::IsNearlyEqual(resourceCount, cap) || resourceCount > cap;
}

void UResourceInventory::OnDayBegin()
{
	_canAddResources = true;
}

void UResourceInventory::OnDayEnd()
{
	if (_clearResourcesAtEndOfDay)
	{
		ClearAllExceptMoney();
	}

	if (!_canAlwaysAddResources)
	{
		_canAddResources = false;
	}
}

void UResourceInventory::ClearAllExceptMoney()
{
	for (auto pair : _resourcesMap)
	{
		const FGameplayTag& resourceType = pair.Key;
		if (!resourceType.MatchesTag(ResourceTypeTags::Money))
		{
			SetResourceAmount(resourceType, 0);
		}
	}
}

