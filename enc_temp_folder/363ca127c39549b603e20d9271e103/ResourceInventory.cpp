// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceInventory.h"

// Brock
#include "DayNightCycleManager.h"
#include "FarmFPSUtilities.h"
#include "ResourceTypeTags.h"

UResourceInventory::UResourceInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceInventory::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &UResourceInventory::OnDayEnd);
	}
}

void UResourceInventory::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UResourceInventory::AddResource(const FGameplayTag& resourceType, float amount)
{
	if (amount > 0)
	{
		CheckInitializeMap(resourceType);

		SetResourceAmount(resourceType, GetResourceCount(resourceType) + amount);
	}
}

void UResourceInventory::SetResourceCap(const FGameplayTag& resourceType, float cap)
{
	if (cap > 0)
	{
		CheckInitializeMap(resourceType);

		_resourceCaps[resourceType] = cap;
	}
}

void UResourceInventory::RemoveResource(const FGameplayTag& resourceType, float amount)
{
	if (amount > 0)
	{
		CheckInitializeMap(resourceType);

		if (ensure(HasResourceAmount(resourceType, amount)))
		{
			SetResourceAmount(resourceType, GetResourceCount(resourceType) - amount);
		}
	}
}

void UResourceInventory::SetResourceAmount(const FGameplayTag& resourceType, float newAmount)
{
	CheckInitializeMap(resourceType);

	if (GetResourceCount(resourceType) == newAmount)
	{
		return;
	}

	_resourcesMap[resourceType] = FMath::Clamp(newAmount, 0 , GetResourceCap(resourceType));

	OnResourceCountChanged.Broadcast(resourceType, GetResourceCount(resourceType));
}

void UResourceInventory::AddAllResourcesInInventory(UResourceInventory* otherInventory)
{
	for (auto pair : otherInventory->_resourcesMap)
	{
		AddResource(pair.Key, pair.Value);
		otherInventory->RemoveResource(pair.Key, pair.Value);
	}
}

bool UResourceInventory::CanAddResource(const FGameplayTag& resourceType, float amount) const
{
	return GetResourceCount(resourceType) + amount <= GetResourceCap(resourceType);
}

float UResourceInventory::GetResourceCount(const FGameplayTag& resourceType) const
{
	if (_resourcesMap.Contains(resourceType))
	{
		return _resourcesMap[resourceType];
	}

	return 0;
}

bool UResourceInventory::HasResourceAmount(const FGameplayTag& resourceType, float amount) const
{
	return GetResourceCount(resourceType) >= amount;
}

uint16 UResourceInventory::GetResourceCap(const FGameplayTag& resourceType) const
{
	return _resourceCaps.Contains(resourceType)  && _resourceCaps[resourceType] > 0 ? _resourceCaps[resourceType] : 999;
}

void UResourceInventory::OnDayEnd()
{
	ClearAllExceptMoney();
}

void UResourceInventory::CheckInitializeMap(const FGameplayTag& resourceType)
{
	if (!_resourcesMap.Contains(resourceType))
	{
		_resourcesMap.Add(resourceType, 0);
	}

	if (!_resourceCaps.Contains(resourceType))
	{
		_resourceCaps.Add(resourceType, 0);
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

