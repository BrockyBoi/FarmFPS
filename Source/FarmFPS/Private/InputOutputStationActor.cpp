// Fill out your copyright notice in the Description page of Project Settings.

#include "InputOutputStationActor.h"

AInputOutputStationActor::AInputOutputStationActor()
{
	PrimaryActorTick.bCanEverTick = false;

	_resourceInputPoint = CreateDefaultSubobject<UAutomaticResourceTransferPoint>("ResourceInputPoint");
	_resourceOutputPoint = CreateDefaultSubobject<UAutomaticResourceTransferPoint>("ResourceOutputPoint");

	_inputInventory = CreateDefaultSubobject<UResourceInventory>("InputInventory");
	_outputInventory = CreateDefaultSubobject<UResourceInventory>("OutputInventory");
}

void AInputOutputStationActor::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_resourceInputPoint)) && ensure(IsValid(_resourceOutputPoint)) && ensure(IsValid(_inputInventory)))
	{
		_resourceInputPoint->SetInventory(_inputInventory);
		_resourceOutputPoint->SetInventory(_outputInventory);

		_inputInventory->OnResourceCountChanged.AddDynamic(this, &AInputOutputStationActor::OnInputInventoryResourceCountChanged);
		_outputInventory->OnResourceCountChanged.AddDynamic(this, &AInputOutputStationActor::OnOutputInventoryResourceCountChanged);
	}
}

void AInputOutputStationActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_inputInventory) && IsValid(_outputInventory))
	{
		_inputInventory->OnResourceCountChanged.RemoveAll(this);
		_outputInventory->OnResourceCountChanged.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AInputOutputStationActor::OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, int amount)
{
}

void AInputOutputStationActor::OnOutputInventoryResourceCountChanged(const FGameplayTag& resourceType, int amount)
{
}
