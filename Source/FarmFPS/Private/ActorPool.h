// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PooledActorTypes.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ActorPool.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UActorPool : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorPool();

	void InitializeActors(const FGameplayTag& actorTag, int actorCount, EPooledActorType actorType);

	void AddActorToPool(const FGameplayTag& actorTag, AActor* actor, EPooledActorType actorType);
	AActor* GetActorFromPool(const FGameplayTag& actorTag, const FTransform& spawnTransform, EPooledActorType actorType);
	AActor* GetActorFromPool(const FGameplayTag& actorTag, const FVector& spawnLocation, EPooledActorType actorType);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void SpawnNewActor(const FGameplayTag& actorTag, const FTransform& spawnTransform, EPooledActorType actorType);
	void SpawnNewActor(const FGameplayTag& actorTag, const FVector& spawnLocation, EPooledActorType actorType);

	TMap<FGameplayTag, TArray<AActor*>>& GetPoolMap(EPooledActorType actorType);

	TMap<FGameplayTag, TArray<AActor*>> _pooledProjectiles;
	TMap<FGameplayTag, TArray<AActor*>> _pooledResourceActors;
	TMap<FGameplayTag, TArray<AActor*>> _pooledCropActors;
	TMap<FGameplayTag, TArray<AActor*>> _emptyPool;
};
