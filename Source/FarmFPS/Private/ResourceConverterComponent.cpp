// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterComponent.h"

// Brock
#include "CraftingData.h"
#include "ResourceInventory.h"

UResourceConverterComponent::UResourceConverterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UResourceConverterComponent::TryConvertResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, FCraftingData& recipeToCraft)
{
	if (!ensure(IsValid(inputInventory)) || !ensure(IsValid(outputInventory)))
	{
		return false;
	}

	for (auto pair : recipeToCraft.RequiredResources)
	{
		EResourceType resourceType = pair.Key;
		uint16 requiredAmount = pair.Value;
		if (!inputInventory->HasResourceAmount(resourceType, requiredAmount))
		{
			return false;
		}
	}

	for (auto pair : recipeToCraft.RequiredResources)
	{
		EResourceType resourceType = pair.Key;
		uint16 requiredAmount = pair.Value;

		inputInventory->RemoveResource(resourceType, requiredAmount);
	}

	outputInventory->AddResource(recipeToCraft.ResourceProductType, 1);
	return true;
}


void UResourceConverterComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

