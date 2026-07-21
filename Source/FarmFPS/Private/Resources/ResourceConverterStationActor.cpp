// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterStationActor.h"

// Brock
#include "Interactables/AutomaticResourceTransferPoint.h"
#include "Resources/ResourceConverterComponent.h"
#include "Resources/ResourceInventory.h"

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
			_resourceConverter->TryConvertAllResources(_inputInventory, _outputInventory, craftingData);
		}
	}
}

void AResourceConverterStationActor::TryConvertLimitedAmount(int amountToCraft)
{
	if (ensure(IsValid(_resourceConverter)))
	{
		for (FCraftingData& craftingData : _craftingRecipes)
		{
			_resourceConverter->TryConvertResources(_inputInventory, _outputInventory, craftingData, amountToCraft);

		}
	}
}

void AResourceConverterStationActor::OnInputInventoryResourceCountChanged(const FGameplayTag&, float)
{
	if (_automaticallyConvertResources)
	{
		TryConvertAllResources();
	}
}
