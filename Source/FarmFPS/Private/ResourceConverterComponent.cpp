// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterComponent.h"

// Brock
#include "CraftingData.h"
#include "FarmFPSUtilities.h"
#include "ObjectiveManager.h"
#include "ObjectiveTypeTags.h"
#include "ResourceInventory.h"

UResourceConverterComponent::UResourceConverterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceConverterComponent::BeginPlay()
{
	Super::BeginPlay();
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

bool UResourceConverterComponent::CanCreateResource(const UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const
{
	for (auto pair : recipeToCraft.RequiredResources)
	{
		FModifiedResourceValue resourceValue = pair.Key;
		uint16 requiredAmount = FarmFPSUtilities::GetModifiedValueByPlayerPerks(this, resourceValue.Modifiers, pair.Value);
		if (!inputInventory->HasResourceAmount(resourceValue.ResourceTag, requiredAmount))
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
			FModifiedResourceValue resourceValue = pair.Key;
			uint16 requiredAmount = FarmFPSUtilities::GetModifiedValueByPlayerPerks(this, resourceValue.Modifiers, pair.Value);
			inputInventory->RemoveResource(resourceValue.ResourceTag, requiredAmount * resourcesToCreate);
		}

		int outputCount = resourcesToCreate * FarmFPSUtilities::GetModifiedValueByPlayerPerks(this, recipeToCraft.ResourceProduct.Modifiers, recipeToCraft.ResourceProductCount);
		FGameplayTag outputResourceType = recipeToCraft.ResourceProduct.ResourceTag;
		outputInventory->AddResource(outputResourceType, outputCount);

		UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
		if (ensure(IsValid(objectiveManager)))
		{
			objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::CraftResource, outputResourceType, outputCount);
		}
	}
}

int UResourceConverterComponent::GetMaxAmountOfResourceCanBeCrafted(const UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const
{
	int maxCanMake = INT_MAX;
	for (auto pair : recipeToCraft.RequiredResources)
	{
		FModifiedResourceValue resourceValue = pair.Key;
		uint16 requiredAmount = FarmFPSUtilities::GetModifiedValueByPlayerPerks(this, resourceValue.Modifiers, pair.Value);

		int timesCanConvert = inputInventory->GetResourceCount(resourceValue.ResourceTag) / requiredAmount;
		if (timesCanConvert < maxCanMake)
		{
			maxCanMake = timesCanConvert;
		}
	}

	return maxCanMake;
}

