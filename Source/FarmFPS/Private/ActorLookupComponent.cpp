// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorLookupComponent.h"

// Brock
#include "CropActorData.h"
#include "PooledActorTypes.h"
#include "ProjectileActorData.h"
#include "ResourceActorData.h"

UActorLookupComponent::UActorLookupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TSubclassOf<AActor> UActorLookupComponent::GetActorReference(const FGameplayTag& actorTag, EPooledActorType actorType) const
{
	switch (actorType)
	{
		case EPooledActorType::Projectile:
			return GetProjectileActor(actorTag);
		case EPooledActorType::ResourcePickup:
			return GetResourceActor(actorTag);
		case EPooledActorType::Crop:
			return GetCropActor(actorTag);
		default:
			return TSubclassOf<AActor>();
	}
}

TSubclassOf<AResourcePickupActor> UActorLookupComponent::GetResourceActor(const FGameplayTag& resourceType) const
{
	if (ensure(_resourceActorMap.Contains(resourceType)))
	{
		return _resourceActorMap[resourceType];
	}

	return TSubclassOf<AResourcePickupActor>();
}

TSubclassOf<AShooterProjectile> UActorLookupComponent::GetProjectileActor(const FGameplayTag& resourceType) const
{
	if (ensure(_projectileActorMap.Contains(resourceType)))
	{
		return _projectileActorMap[resourceType];
	}

	return TSubclassOf<AShooterProjectile>();
}

TSubclassOf<AActor> UActorLookupComponent::GetCropActor(const FGameplayTag& cropType) const
{
	if (ensure(_cropActorMap.Contains(cropType)))
	{
		return _cropActorMap[cropType];
	}
	
	return TSubclassOf<AActor>();
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
				_resourceActorMap.Add(row->ResourceType, row->ResourceActorClass);
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
				_projectileActorMap.Add(row->ProjectileType, row->ProjectileActorClass);
			}
		}
	}

	if (ensure(IsValid(_cropActorTable)))
	{
		// Iterate through the internal row map directly
		for (auto It = _cropActorTable->GetRowMap().CreateConstIterator(); It; ++It)
		{
			FName RowName = It.Key();
			FCropActorData* row = reinterpret_cast<FCropActorData*>(It.Value());

			if (ensure(row))
			{
				_cropActorMap.Add(row->ResourceType, row->CropActorClass);
			}
		}
	}
}