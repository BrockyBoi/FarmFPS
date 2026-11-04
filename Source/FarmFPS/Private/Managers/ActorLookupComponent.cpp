// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorLookupComponent.h"

// Brock
#include "Data/CropActorData.h"
#include "Data/ProjectileActorData.h"
#include "Data/ResourceActorData.h"
#include "Managers/PooledActorTypes.h"

DECLARE_CYCLE_STAT(TEXT("ActorLookupCheck"), STAT_ActorLookupCheck, STATGROUP_ActorLookup);

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
	SCOPE_CYCLE_COUNTER(STAT_ActorLookupCheck);

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
	if (ensure(_resourceActorMap.Contains(resourceType)) && ensure(!_resourceActorMap[resourceType].ResourceActorClass.IsNull()))
	{
		TSoftClassPtr<AResourcePickupActor> actorClass = _resourceActorMap[resourceType].ResourceActorClass;
		return actorClass.IsValid() ? actorClass.Get() : actorClass.LoadSynchronous();
	}

	return TSubclassOf<AResourcePickupActor>();
}

TSubclassOf<AShooterProjectile> UActorLookupComponent::GetProjectileActor(const FGameplayTag& resourceType) const
{
	if (ensure(_resourceActorMap.Contains(resourceType)) && ensure(!_resourceActorMap[resourceType].ProjectileActorClass.IsNull()))
	{
		TSoftClassPtr<AShooterProjectile> actorClass = _resourceActorMap[resourceType].ProjectileActorClass;
		return actorClass.IsValid() ? actorClass.Get() : actorClass.LoadSynchronous();
	}

	return TSubclassOf<AShooterProjectile>();
}

TSubclassOf<ACrop> UActorLookupComponent::GetCropActor(const FGameplayTag& cropType) const
{
	if (ensure(_resourceActorMap.Contains(cropType)) && ensure(!_resourceActorMap[cropType].CropActorClass.IsNull()))
	{
		TSoftClassPtr<ACrop> actorClass = _resourceActorMap[cropType].CropActorClass;
		return actorClass.IsValid() ? actorClass.Get() : actorClass.LoadSynchronous();
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