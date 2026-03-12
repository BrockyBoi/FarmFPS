// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConverterStationActor.h"

// Brock
#include "AutomaticResourceTransferPoint.h"
#include "ResourceConverterComponent.h"
#include "ResourceInventory.h"

AResourceConverterStationActor::AResourceConverterStationActor()
{
	PrimaryActorTick.bCanEverTick = false;

	_resourceConverter = CreateDefaultSubobject<UResourceConverterComponent>("ResourceConverterComponent");

	_resourceInputPoint = CreateDefaultSubobject<UAutomaticResourceTransferPoint>("ResourceInputPoint");
	_resourceOutputPoint = CreateDefaultSubobject<UAutomaticResourceTransferPoint>("ResourceOutputPoint");

	_inputInventory = CreateDefaultSubobject<UResourceInventory>("InputInventory");
	_outputInventory = CreateDefaultSubobject<UResourceInventory>("OutputInventory");
}

// Called when the game starts or when spawned
void AResourceConverterStationActor::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_resourceInputPoint)) && ensure(IsValid(_resourceOutputPoint)) && ensure(IsValid(_inputInventory)))
	{
		_resourceInputPoint->SetInventory(_inputInventory);
		_resourceOutputPoint->SetInventory(_outputInventory);

		_inputInventory->OnResourceCountChanged.AddDynamic(this, &AResourceConverterStationActor::OnInputInventoryResourceCountChanged);
	}

	if (!ensure(_craftingRecipe.RequiredResources.Num() > 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not have any required resouces for recipe"), *GetName());
	}
}

void AResourceConverterStationActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_inputInventory))
	{
		_inputInventory->OnResourceCountChanged.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AResourceConverterStationActor::OnInputInventoryResourceCountChanged(const FGameplayTag&, int)
{
	_resourceConverter->TryConvertResources(_inputInventory, _outputInventory, _craftingRecipe);
}
