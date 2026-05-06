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

void UActorPool::InitializeActors(const FGameplayTag& actorTag, int actorCount)
{
	if (!_pooledActors.Contains(actorTag))
	{
		_pooledActors.Add(actorTag);
	}

	for (int i = 0; i < actorCount; ++i)
	{
		SpawnNewActor(actorTag, FVector::ZeroVector);
	}
}

AActor* UActorPool::GetActorFromPool(const FGameplayTag& actorTag, const FTransform& spawnTransform)
{
	if (!_pooledActors.Contains(actorTag))
	{
		_pooledActors.Add(actorTag);
	}

	if (_pooledActors[actorTag].IsEmpty())
	{
		SpawnNewActor(actorTag, spawnTransform);
	}

	AActor* pooledActor = _pooledActors[actorTag].Pop();
	if (ensure(IsValid(pooledActor)))
	{
		pooledActor->SetActorHiddenInGame(false);
		pooledActor->SetActorEnableCollision(true);
		pooledActor->SetActorTransform(spawnTransform);
		return pooledActor;
	}

	return nullptr;
}

AActor* UActorPool::GetActorFromPool(const FGameplayTag& actorTag, const FVector& spawnLocation)
{
	if (!_pooledActors.Contains(actorTag))
	{
		_pooledActors.Add(actorTag);
	}

	if (_pooledActors[actorTag].IsEmpty())
	{
		SpawnNewActor(actorTag, spawnLocation);
	}

	AActor* pooledActor = _pooledActors[actorTag].Pop();
	if (ensure(IsValid(pooledActor)))
	{
		pooledActor->SetActorHiddenInGame(false);
		pooledActor->SetActorEnableCollision(true);
		pooledActor->SetActorLocation(spawnLocation);
		return pooledActor;
	}

	return nullptr;
}

void UActorPool::AddActorToPool(const FGameplayTag& actorTag, AActor* actor)
{
	if (!_pooledActors.Contains(actorTag))
	{
		_pooledActors.Add(actorTag);
	}

	if (ensure(IsValid(actor)))
	{
		_pooledActors[actorTag].Add(actor);
		actor->SetActorEnableCollision(false);
		actor->SetActorHiddenInGame(true);

		IPoolableActor* poolableActor = Cast<IPoolableActor>(actor);
		if (ensure(poolableActor))
		{
			poolableActor->AddActorToPool();
		}
	}
}

void UActorPool::SpawnNewActor(const FGameplayTag& actorTag, const FTransform& spawnTransform)
{
	UActorLookupComponent* lookupComponent = FarmFPSUtilities::GetResourceActorLookupComponent(this);
	if (ensure(IsValid(lookupComponent)))
	{
		TSubclassOf<AActor> resourceActor = lookupComponent->GetActorForType(actorTag);
		if (ensure(IsValid(resourceActor)))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* actor = GetWorld()->SpawnActor<AActor>(resourceActor, spawnTransform, SpawnParams);

			if (ensure(IsValid(actor)))
			{
				AddActorToPool(actorTag, actor);
			}
		}
	}
}

void UActorPool::SpawnNewActor(const FGameplayTag& actorTag, const FVector& spawnLocation)
{
	UActorLookupComponent* lookupComponent = FarmFPSUtilities::GetResourceActorLookupComponent(this);
	if (ensure(IsValid(lookupComponent)))
	{
		TSubclassOf<AActor> resourceActor = lookupComponent->GetActorForType(actorTag);
		if (ensure(IsValid(resourceActor)))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* actor = GetWorld()->SpawnActor<AActor>(resourceActor, spawnLocation, FRotator::ZeroRotator, SpawnParams);

			if (ensure(IsValid(actor)))
			{
				AddActorToPool(actorTag, actor);
			}
		}
	}
}
