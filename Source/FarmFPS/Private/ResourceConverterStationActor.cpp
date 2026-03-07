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

	//_resourceInput = CreateDefaultSubobject<UResourceInventory>("ResourceInput");
	//_resourceOutput = CreateDefaultSubobject<UResourceInventory>("ResourceOutput");
}

// Called when the game starts or when spawned
void AResourceConverterStationActor::BeginPlay()
{
	Super::BeginPlay();
	
}