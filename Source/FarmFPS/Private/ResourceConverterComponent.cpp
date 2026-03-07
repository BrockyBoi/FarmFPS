// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterComponent.h"

// Brock
#include "CraftingData.h"
#include "ResourceInventory.h"

UResourceConverterComponent::UResourceConverterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UResourceConverterComponent::TryConvertResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft)
{
	if (!ensure(IsValid(inputInventory)) || !ensure(IsValid(outputInventory)))
	{
		return false;
	}

	if (!CanCreateResource(inputInventory, recipeToCraft))
	{
		return false;
	}

	ConvertAllResourcesPossible(inputInventory, outputInventory, recipeToCraft);
	return true;
}


void UResourceConverterComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UResourceConverterComponent::CanCreateResource(UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const
{
	for (auto pair : recipeToCraft.RequiredResources)
	{
		EResourceType resourceType = pair.Key;
		uint16 requiredAmount = pair.Value;
		if (!inputInventory->HasResourceAmount(resourceType, requiredAmount))
		{
			return false;
		}
	}

	return true;
}

void UResourceConverterComponent::ConvertAllResourcesPossible(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft)
{
	int resourcesToCreate = GetMaxAmountOfResourceCanBeCrafted(inputInventory, recipeToCraft);
	if (resourcesToCreate > 0)
	{
		for (auto pair : recipeToCraft.RequiredResources)
		{
			EResourceType resourceType = pair.Key;
			uint16 requiredAmount = pair.Value;
			inputInventory->RemoveResource(resourceType, requiredAmount * resourcesToCreate);
		}
		outputInventory->AddResource(recipeToCraft.ResourceProductType, resourcesToCreate* recipeToCraft.ResourceProductCount);
	}
}

int UResourceConverterComponent::GetMaxAmountOfResourceCanBeCrafted(UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const
{
	int maxCanMake = INT_MAX;
	for (auto pair : recipeToCraft.RequiredResources)
	{
		EResourceType resourceType = pair.Key;
		uint16 requiredAmount = pair.Value;

		int timesCanConvert = inputInventory->GetResourceCount(resourceType) / requiredAmount;
		if (timesCanConvert < maxCanMake)
		{
			maxCanMake = timesCanConvert;
		}
	}

	return maxCanMake;
}

