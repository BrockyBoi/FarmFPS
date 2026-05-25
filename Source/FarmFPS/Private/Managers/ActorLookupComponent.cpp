// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorLookupComponent.h"

// Brock
#include "Plants/CropActorData.h"
#include "Managers/PooledActorTypes.h"
#include "Projectiles/ProjectileActorData.h"
#include "Resources/ResourceActorData.h"

UActorLookupComponent::UActorLookupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorLookupComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeMapFromTables();
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
		return _resourceActorMap[resourceType].ResourceActorClass;
	}

	return TSubclassOf<AResourcePickupActor>();
}

TSubclassOf<AShooterProjectile> UActorLookupComponent::GetProjectileActor(const FGameplayTag& resourceType) const
{
	if (ensure(_resourceActorMap.Contains(resourceType)))
	{
		return _resourceActorMap[resourceType].ProjectileActorClass;
	}

	return TSubclassOf<AShooterProjectile>();
}

TSubclassOf<ACrop> UActorLookupComponent::GetCropActor(const FGameplayTag& cropType) const
{
	if (ensure(_resourceActorMap.Contains(cropType)))
	{
		return _resourceActorMap[cropType].CropActorClass;
	}

	return TSubclassOf<ACrop>();
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
				_resourceActorMap.Add(row->ResourceType, *row);
			}
		}
	}
}