// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceInventory.h"

UResourceInventory::UResourceInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceInventory::BeginPlay()
{
	Super::BeginPlay();
}

void UResourceInventory::AddResource(const FGameplayTag& resourceType, uint16 amount)
{
	if (amount > 0)
	{
		CheckInitializeMap(resourceType);

		SetResourceAmount(resourceType, GetResourceCount(resourceType) + amount);
	}
}

void UResourceInventory::RemoveResource(const FGameplayTag& resourceType, uint16 amount)
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

void UResourceInventory::SetResourceAmount(const FGameplayTag& resourceType, uint16 newAmount)
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

bool UResourceInventory::CanAddResource(const FGameplayTag& resourceType, uint16 amount) const
{
	return GetResourceCount(resourceType) + amount <= GetResourceCap(resourceType);
}

int UResourceInventory::GetResourceCount(const FGameplayTag& resourceType) const
{
	if (_resourcesMap.Contains(resourceType))
	{
		return _resourcesMap[resourceType];
	}

	return 0;
}

bool UResourceInventory::HasResourceAmount(const FGameplayTag& resourceType, int amount) const
{
	return GetResourceCount(resourceType) >= amount;
}

uint16 UResourceInventory::GetResourceCap(const FGameplayTag& resourceType) const
{
	return _resourceCaps.Contains(resourceType) ? _resourceCaps[resourceType] : 999;
}

void UResourceInventory::CheckInitializeMap(const FGameplayTag& resourceType)
{
	if (!_resourcesMap.Contains(resourceType))
	{
		_resourcesMap.Add(resourceType, 0);
	}
}

