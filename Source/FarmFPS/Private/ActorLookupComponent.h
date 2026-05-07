// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PooledActorTypes.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ActorLookupComponent.generated.h"

class ACrop;
class AResourcePickupActor;
class AShooterProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UActorLookupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorLookupComponent();

	TSubclassOf<AActor> GetActorReference(const FGameplayTag& actorTag, EPooledActorType actorType) const;

protected:
	virtual void BeginPlay() override;	
	void InitializeMapFromTables();

private:

	TSubclassOf<AResourcePickupActor> GetResourceActor(const FGameplayTag& resourceType) const;
	TSubclassOf<AShooterProjectile> GetProjectileActor(const FGameplayTag& resourceType) const;
	TSubclassOf<AActor> GetCropActor(const FGameplayTag& cropType) const;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _resourceActorTable;
	TMap<FGameplayTag, TSubclassOf<AResourcePickupActor>> _resourceActorMap;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _projectileActorTable;
	TMap<FGameplayTag, TSubclassOf<AShooterProjectile>> _projectileActorMap;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _cropActorTable;
	TMap<FGameplayTag, TSubclassOf<ACrop>> _cropActorMap;
};
