// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerInventoryItemSelector.h"

// Brock
#include "Resources/ResourceInventory.h"
#include "Resources/ResourceTypeTags.h"

UPlayerInventoryItemSelector::UPlayerInventoryItemSelector()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerInventoryItemSelector::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerInventoryItemSelector::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_playerInventory))
	{
		_playerInventory->OnResourceCountChanged.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UPlayerInventoryItemSelector::IncreaseIndex()
{
	if (_currentInventoryItemTypes.Num() == 0)
	{
		return;
	}

	SetIndexValue((_currentSelectedIndex + 1) % _currentInventoryItemTypes.Num());
}

void UPlayerInventoryItemSelector::DecreaseIndex()
{
	if (_currentInventoryItemTypes.Num() == 0)
	{
		return;
	}

	SetIndexValue((_currentSelectedIndex - 1 + _currentInventoryItemTypes.Num()) % _currentInventoryItemTypes.Num());
}

void UPlayerInventoryItemSelector::SetIndexValue(int index)
{
	if (_currentInventoryItemTypes.Num() == 0)
	{
		return;
	}

	if (_currentInventoryItemTypes.IsValidIndex(index))
	{
		_currentSelectedIndex = index;
	}
}

void UPlayerInventoryItemSelector::SetPlayerInventory(UResourceInventory* playerInventory)
{
	if (ensure(IsValid(playerInventory)))
	{
		_playerInventory = playerInventory;
		playerInventory->OnResourceCountChanged.AddDynamic(this, &UPlayerInventoryItemSelector::OnResourceChanged);
		SetCurrentInventoryItemTypes();
	}
}

void UPlayerInventoryItemSelector::OnResourceChanged(const FGameplayTag& resourceType, float newAmount)
{
	if (resourceType == ResourceTypeTags::Money)
	{
		return;
	}

	const int startingItemTypeCount = _currentInventoryItemTypes.Num();

	if (_currentInventoryItemTypes.Contains(resourceType) && newAmount <= 0)
	{
		_currentInventoryItemTypes.Remove(resourceType);
	}
	else if (!_currentInventoryItemTypes.Contains(resourceType) && newAmount > 0)
	{
		_currentInventoryItemTypes.Add(resourceType);
	}

	if (startingItemTypeCount > 0 && _currentSelectedIndex >= _currentInventoryItemTypes.Num())
	{
		_currentSelectedIndex = _currentInventoryItemTypes.Num() - 1;
	}
	else if(startingItemTypeCount == 0 && _currentInventoryItemTypes.Num() > 0)
	{
		_currentSelectedIndex = 0;
	}

	if (_currentInventoryItemTypes.Num() == 0)
	{
		_currentSelectedIndex = -1;
	}
}

void UPlayerInventoryItemSelector::SetCurrentInventoryItemTypes()
{
	if (ensure(IsValid(_playerInventory)))
	{
		_currentInventoryItemTypes.Empty();
		for (const TPair<FGameplayTag, float>& resource : _playerInventory->GetResourcesMap())
		{
			if (resource.Value > 0)
			{
				_currentInventoryItemTypes.Add(resource.Key);
			}
		}

		if (_currentInventoryItemTypes.Num() > 0)
		{
			_currentSelectedIndex = 0;
		}
	}
}
