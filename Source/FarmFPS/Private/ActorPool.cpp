// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPool.h"

// Brock
#include "ActorLookupComponent.h"
#include "FarmFPSUtilities.h"
#include "PoolableActor.h"

UActorPool::UActorPool()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorPool::BeginPlay()
{
	Super::BeginPlay();
}

void UActorPool::InitializeActors(const FGameplayTag& actorTag, int actorCount, EPooledActorType actorType)
{
	TMap<FGameplayTag, TArray<AActor*>>& pooledActors = GetPoolMap(actorType);
	if (!pooledActors.Contains(actorTag))
	{
		pooledActors.Add(actorTag);
	}

	for (int i = 0; i < actorCount; ++i)
	{
		SpawnNewActor(actorTag, FVector::ZeroVector, actorType);
	}
}

AActor* UActorPool::GetActorFromPool(const FGameplayTag& actorTag, const FTransform& spawnTransform, EPooledActorType actorType)
{
	TMap<FGameplayTag, TArray<AActor*>>& mapReference = GetPoolMap(actorType);

	if (!mapReference.Contains(actorTag))
	{
		mapReference.Add(actorTag);
	}

	if (mapReference[actorTag].IsEmpty())
	{
		SpawnNewActor(actorTag, spawnTransform, actorType);
	}

	AActor* pooledActor = mapReference[actorTag].Pop();
	if (ensure(IsValid(pooledActor)))
	{
		pooledActor->SetActorHiddenInGame(false);
		pooledActor->SetActorEnableCollision(true);
		pooledActor->SetActorTransform(spawnTransform);
		IPoolableActor* poolableActor = Cast<IPoolableActor>(pooledActor);
		if (ensure(pooledActor->Implements<UPoolableActor>()))
		{
			poolableActor->RemoveFromPool();
		}

		return pooledActor;
	}

	return nullptr;
}

AActor* UActorPool::GetActorFromPool(const FGameplayTag& actorTag, const FVector& spawnLocation, EPooledActorType actorType)
{
	TMap<FGameplayTag, TArray<AActor*>>& mapReference = GetPoolMap(actorType);
	if (!mapReference.Contains(actorTag))
	{
		mapReference.Add(actorTag);
	}

	if (mapReference[actorTag].IsEmpty())
	{
		SpawnNewActor(actorTag, spawnLocation, actorType);
	}

	AActor* pooledActor = mapReference[actorTag].Pop();
	if (ensure(IsValid(pooledActor)))
	{
		pooledActor->SetActorHiddenInGame(false);
		pooledActor->SetActorEnableCollision(true);
		pooledActor->SetActorLocation(spawnLocation);

		IPoolableActor* poolableActor = Cast<IPoolableActor>(pooledActor);
		if (ensure(pooledActor->Implements<UPoolableActor>()))
		{
			poolableActor->RemoveFromPool();
		}
		return pooledActor;
	}

	return nullptr;
}

void UActorPool::AddActorToPool(const FGameplayTag& actorTag, AActor* actor, EPooledActorType actorType)
{
	TMap<FGameplayTag, TArray<AActor*>>& pooledActors = GetPoolMap(actorType);

	if (!pooledActors.Contains(actorTag))
	{
		pooledActors.Add(actorTag);
	}

	if (ensure(IsValid(actor)))
	{
		pooledActors[actorTag].Add(actor);
		actor->SetActorEnableCollision(false);
		actor->SetActorHiddenInGame(true);

		IPoolableActor* poolableActor = Cast<IPoolableActor>(actor);
		if (ensure(actor->Implements<UPoolableActor>()))
		{
			poolableActor->AddActorToPool();
		}
	}
}

void UActorPool::SpawnNewActor(const FGameplayTag& actorTag, const FTransform& spawnTransform, EPooledActorType actorType)
{
	UActorLookupComponent* lookupComponent = FarmFPSUtilities::GetResourceActorLookupComponent(this);
	if (ensure(IsValid(lookupComponent)))
	{
		TSubclassOf<AActor> resourceActor = lookupComponent->GetActorReference(actorTag, actorType);
		if (ensure(IsValid(resourceActor)))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* actor = GetWorld()->SpawnActor<AActor>(resourceActor, spawnTransform, SpawnParams);

			if (ensure(IsValid(actor)))
			{
				AddActorToPool(actorTag, actor, actorType);
			}
		}
	}
}

void UActorPool::SpawnNewActor(const FGameplayTag& actorTag, const FVector& spawnLocation, EPooledActorType actorType)
{
	UActorLookupComponent* lookupComponent = FarmFPSUtilities::GetResourceActorLookupComponent(this);
	if (ensure(IsValid(lookupComponent)))
	{
		TSubclassOf<AActor> resourceActor = lookupComponent->GetActorReference(actorTag, actorType);
		if (ensure(IsValid(resourceActor)))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* actor = GetWorld()->SpawnActor<AActor>(resourceActor, spawnLocation, FRotator::ZeroRotator, SpawnParams);

			if (ensure(IsValid(actor)))
			{
				AddActorToPool(actorTag, actor, actorType);
			}
		}
	}
}

TMap<FGameplayTag, TArray<AActor*>>& UActorPool::GetPoolMap(EPooledActorType actorType)
{
	switch (actorType)
	{
		case EPooledActorType::Projectile:
			return _pooledProjectiles;
		case EPooledActorType::ResourcePickup:
			return _pooledResourceActors;
		case EPooledActorType::Crop:
			return _pooledCropActors;
		default:
			return _emptyPool;
	}
}
