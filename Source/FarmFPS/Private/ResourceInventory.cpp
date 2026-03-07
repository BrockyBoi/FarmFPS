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

void UResourceInventory::AddResource(EResourceType resourceType, uint16 amount)
{
	if (amount > 0)
	{
		CheckInitializeMap(resourceType);

		SetResourceAmount(resourceType, GetResourceCount(resourceType) + amount);
	}
}

void UResourceInventory::RemoveResource(EResourceType resourceType, uint16 amount)
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

void UResourceInventory::SetResourceAmount(EResourceType resourceType, uint16 newAmount)
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

bool UResourceInventory::CanAddResource(EResourceType resourceType, uint16 amount) const
{
	return GetResourceCount(resourceType) + amount <= GetResourceCap(resourceType);
}

int UResourceInventory::GetResourceCount(EResourceType resourceType) const
{
	if (_resourcesMap.Contains(resourceType))
	{
		return _resourcesMap[resourceType];
	}

	return 0;
}

bool UResourceInventory::HasResourceAmount(EResourceType cropType, int amount) const
{
	return GetResourceCount(cropType) >= amount;
}

uint16 UResourceInventory::GetResourceCap(EResourceType resourceType) const
{
	return _resourceCaps.Contains(resourceType) ? _resourceCaps[resourceType] : BIG_NUMBER;
}

void UResourceInventory::CheckInitializeMap(EResourceType cropType)
{
	if (!_resourcesMap.Contains(cropType))
	{
		_resourcesMap.Add(cropType, 0);
	}
}

