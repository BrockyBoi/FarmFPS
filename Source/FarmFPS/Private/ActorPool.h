// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock

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

	void InitializeActors(const FGameplayTag& actorTag, int actorCount);

	AActor* GetActorFromPool(const FGameplayTag& actorTag, const FTransform& spawnTransform);
	AActor* GetActorFromPool(const FGameplayTag& actorTag, const FVector& spawnLocation);
	void AddActorToPool(const FGameplayTag& actorTag, AActor* actor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SpawnNewActor(const FGameplayTag& actorTag, const FTransform& spawnTransform);
	void SpawnNewActor(const FGameplayTag& actorTag, const FVector& spawnLocation);

	TMap<FGameplayTag, TArray<AActor*>> _pooledActors;
};
