// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterStationActor.h"

// Brock
#include "AutomaticResourceTransferPoint.h"
#include "ResourceConverterComponent.h"
#include "ResourceInventory.h"

AResourceConverterStationActor::AResourceConverterStationActor() : Super()
{
	_resourceConverter = CreateDefaultSubobject<UResourceConverterComponent>("ResourceConverterComponent");
}

void AResourceConverterStationActor::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(_craftingRecipe.RequiredResources.Num() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not have any required resouces for recipe"), *GetName());
	}
}

void AResourceConverterStationActor::TryConvertAllResources()
{
	if (ensure(IsValid(_resourceConverter)))
	{
		_resourceConverter->TryConvertAllResources(_inputInventory, _outputInventory, _craftingRecipe);
	}
}

void AResourceConverterStationActor::TryConvertLimitedAmount(int amountToCraft)
{
	if (ensure(IsValid(_resourceConverter)))
	{
		_resourceConverter->TryConvertResources(_inputInventory, _outputInventory, _craftingRecipe, amountToCraft);
	}
}

void AResourceConverterStationActor::OnInputInventoryResourceCountChanged(const FGameplayTag&, float)
{
	if (_automaticallyConvertResources)
	{
		TryConvertAllResources();
	}
}
