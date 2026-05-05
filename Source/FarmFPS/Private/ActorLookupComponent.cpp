// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorLookupComponent.h"

// Brock
#include "ProjectileActorData.h"
#include "ResourceActorData.h"

UActorLookupComponent::UActorLookupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorLookupComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeMapFromTables();
}

void UActorLookupComponent::InitializeMapFromTables()
{
	if (ensure(IsValid(_resourceActorTable)))
	{
		// Iterate through the internal row map directly
		for (auto It = _resourceActorTable->GetRowMap().CreateConstIterator(); It; ++It)
		{
			FName RowName = It.Key();
			FResourceActorData* row = reinterpret_cast<FResourceActorData*>(It.Value());

			if (ensure(row))
			{
				_actorTypeToActorMap.Add(row->ResourceType, row->ResourceActorClass);
			}
		}
	}

	if (ensure(IsValid(_projectileActorTable)))
	{
		// Iterate through the internal row map directly
		for (auto It = _projectileActorTable->GetRowMap().CreateConstIterator(); It; ++It)
		{
			FName RowName = It.Key();
			FProjectileActorData* row = reinterpret_cast<FProjectileActorData*>(It.Value());

			if (ensure(row))
			{
				_actorTypeToActorMap.Add(row->ProjectileType, row->ProjectileActorClass);
			}
		}
	}
}

TSubclassOf<AActor> UActorLookupComponent::GetActorForType(const FGameplayTag& resourceType) const
{
	if (ensure(_actorTypeToActorMap.Contains(resourceType)))
	{
		return _actorTypeToActorMap[resourceType];
	}

	return TSubclassOf<AActor>();
}


