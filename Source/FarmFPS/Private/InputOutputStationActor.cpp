// Fill out your copyright notice in the Description page of Project Settings.

#include "InputOutputStationActor.h"

// Brock
#include "ActorPool.h"
#include "FarmFPSUtilities.h"

AInputOutputStationActor::AInputOutputStationActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = root;

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

void AInputOutputStationActor::Tick(float DeltaTime)
{
	if (_currentSpawnTime >= GetTimeBetweenSpawns())
	{
		if (_resourcesToSpawnFromInputInventory.Num() > 0 && ensure(IsValid(_resourceOutputPoint)))
		{
			for (int i = 0; i < _resourcesToSpawnFromInputInventory.Num(); i++)
			{
				ResourcesToSpawnData& data = _resourcesToSpawnFromInputInventory[i];
				SpawnResource(data);

				if (data.AmountToSpawn <= 0)
				{
					_resourcesToSpawnFromInputInventory.RemoveAt(i);
				}
			}
		}
		else
		{
			SetActorTickEnabled(false);
		}
		_currentSpawnTime = 0.f;
	}
	else
	{
		_currentSpawnTime += DeltaTime;
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

float AInputOutputStationActor::GetTimeBetweenSpawns() const
{
	return _defaultTimeBetweenSpawns.GetModifiedValue(this);
}

void AInputOutputStationActor::SpawnResource(ResourcesToSpawnData& data)
{
	data.AmountToSpawn -= 1;

	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		AActor* pooledActor = actorPool->GetActorFromPool(data.ResourceType, _resourceOutputPoint->GetPlayerCollider()->GetComponentLocation());
		if (IsValid(pooledActor))
		{
			if (ensure(IsValid(pooledActor->FindComponentByClass<UPrimitiveComponent>())))
			{
				pooledActor->FindComponentByClass<UPrimitiveComponent>()->AddImpulse(_launchVector);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get pooled actor for resource type %s"), *data.ResourceType.ToString());
		}
	}
}

void AInputOutputStationActor::OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount)
{

}

void AInputOutputStationActor::OnOutputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount)
{
	if (amount <= 0)
	{
		return;
	}

	if (_outputPhysicalObjectsFromInputInventory)
	{
		bool foundResource = false;
		for (auto& spawnData : _resourcesToSpawnFromInputInventory)
		{
			if (spawnData.ResourceType == resourceType)
			{
				spawnData.AmountToSpawn += amount;
				foundResource = true;
				break;
			}
		}

		if (!foundResource)
		{
			_resourcesToSpawnFromInputInventory.Add({ resourceType, FMath::RoundToInt(amount) });
			SetActorTickEnabled(true);
		}

		_outputInventory->SetResourceAmount(resourceType, 0);
	}
}
