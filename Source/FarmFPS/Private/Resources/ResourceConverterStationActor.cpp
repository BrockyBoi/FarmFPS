// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterStationActor.h"

// Brock
#include "Interactables/AutomaticResourceTransferPoint.h"
#include "Resources/ResourceConverterComponent.h"
#include "Resources/ResourceInventory.h"

FOnStartConvertingResources AResourceConverterStationActor::OnStartConvertingResources;

AResourceConverterStationActor::AResourceConverterStationActor() : Super()
{
	_resourceConverter = CreateDefaultSubobject<UResourceConverterComponent>("ResourceConverterComponent");
}

void AResourceConverterStationActor::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(_craftingRecipes.Num() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not have any required resouces for recipe"), *GetName());
	}

	for (FCraftingData& data : _craftingRecipes)
	{
		if (!ensure(data.RequiredResources.Num() > 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s does not have any required resouces for recipe"), *GetName());
		}
	}
}

void AResourceConverterStationActor::TryConvertAllResources()
{
	if (ensure(IsValid(_resourceConverter)))
	{
		for (FCraftingData& craftingData : _craftingRecipes)
		{
			if (_resourceConverter->TryConvertAllResources(_inputInventory, _outputInventory, craftingData))
			{
				OnStartConvertingResources.Broadcast();
			}
		}
	}
}

void AResourceConverterStationActor::TryConvertLimitedAmount(int amountToCraft)
{
	if (ensure(IsValid(_resourceConverter)))
	{
		for (FCraftingData& craftingData : _craftingRecipes)
		{
			if (_resourceConverter->TryConvertResources(_inputInventory, _outputInventory, craftingData, amountToCraft))
			{
				OnStartConvertingResources.Broadcast();
			}
		}
	}
}

const FCraftingData& AResourceConverterStationActor::GetCurrentCraftingRecipe() const
{
	return _craftingRecipes[_currentRecipeIndex];
}

void AResourceConverterStationActor::OnInputInventoryResourceCountChanged(const FGameplayTag&, float)
{
	if (_automaticallyConvertResources)
	{
		TryConvertAllResources();
	}
}

void AResourceConverterStationActor::SetCurrentRecipeIndex(int index)
{
	if (ensure(_craftingRecipes.Num() > 0))
	{
		_currentRecipeIndex = FMath::Clamp(index, 0, _craftingRecipes.Num() - 1);
	}
}
