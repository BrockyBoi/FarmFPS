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

bool UResourceConverterComponent::TryConvertResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft, int amountToCraft)
{
	if (!ensure(IsValid(inputInventory)) || !ensure(IsValid(outputInventory)))
	{
		return false;
	}

	if (!CanCreateResource(inputInventory, recipeToCraft))
	{
		return false;
	}

	ConvertLimitedResources(inputInventory, outputInventory, recipeToCraft, amountToCraft);
	return true;
}

bool UResourceConverterComponent::TryConvertAllResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft)
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
	for (const FModifiedResourceValue& value : recipeToCraft.RequiredResources)
	{
		uint16 requiredAmount = value.ModifiedIntValue.GetModifiedValue(this);
		if (!inputInventory->HasResourceAmount(value.ResourceTag, requiredAmount))
		{
			return false;
		}
	}

	return true;
}

void UResourceConverterComponent::ConvertLimitedResources(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft, int amountToCraft)
{
	int maxCanCraft = FMath::Clamp(GetMaxAmountOfResourceCanBeCrafted(inputInventory, recipeToCraft), 0, amountToCraft);
	if (maxCanCraft > 0)
	{
		for (const FModifiedResourceValue& value : recipeToCraft.RequiredResources)
		{
			uint16 requiredAmount = value.ModifiedIntValue.GetModifiedValue(this);
			inputInventory->RemoveResource(value.ResourceTag, requiredAmount * maxCanCraft);
		}
		
		for (const FModifiedResourceValue& value : recipeToCraft.ResourceProducts)
		{
			FGameplayTag outputResourceType = value.ResourceTag;
			int outputCount = maxCanCraft * value.ModifiedIntValue.GetModifiedValue(this);

			outputInventory->AddResource(outputResourceType, outputCount);

			UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
			if (ensure(IsValid(objectiveManager)))
			{
				objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::CraftResource, outputResourceType, outputCount);
			}
		}
	}
}

void UResourceConverterComponent::ConvertAllResourcesPossible(UResourceInventory* inputInventory, UResourceInventory* outputInventory, const FCraftingData& recipeToCraft)
{
	int resourcesToCreate = GetMaxAmountOfResourceCanBeCrafted(inputInventory, recipeToCraft);
	if (resourcesToCreate > 0)
	{
		for (const FModifiedResourceValue& value : recipeToCraft.RequiredResources)
		{
			uint16 requiredAmount = value.ModifiedIntValue.GetModifiedValue(this);
			inputInventory->RemoveResource(value.ResourceTag, requiredAmount * resourcesToCreate);
		}

		for (const FModifiedResourceValue& value : recipeToCraft.ResourceProducts)
		{
			FGameplayTag outputResourceType = value.ResourceTag;
			int outputCount = resourcesToCreate * value.ModifiedIntValue.GetModifiedValue(this);

			outputInventory->AddResource(outputResourceType, outputCount);

			UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
			if (ensure(IsValid(objectiveManager)))
			{
				objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::CraftResource, outputResourceType, outputCount);
			}
		}
	}
}

int UResourceConverterComponent::GetMaxAmountOfResourceCanBeCrafted(const UResourceInventory* inputInventory, const FCraftingData& recipeToCraft) const
{
	int maxCanMake = INT_MAX;
	for (const FModifiedResourceValue& value : recipeToCraft.RequiredResources)
	{
		uint16 requiredAmount = value.ModifiedIntValue.GetModifiedValue(this);

		int timesCanConvert = inputInventory->GetResourceCount(value.ResourceTag) / requiredAmount;
		if (timesCanConvert < maxCanMake)
		{
			maxCanMake = timesCanConvert;
		}
	}

	return maxCanMake;
}

