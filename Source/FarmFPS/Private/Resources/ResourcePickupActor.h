// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/PoolableActor.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

// Generated
#include "ResourcePickupActor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCollected, const FGameplayTag&);

class APawn;
class UCapsuleComponent;
class UResourceInventory;
class USphereComponent;

UCLASS()
class AResourcePickupActor : public AActor, public IPoolableActor
{
	GENERATED_BODY()
	
public:	
	AResourcePickupActor();
	virtual void AddActorToPool();
	virtual void RemoveFromPool();
	bool CanBeCollectedByPlayer() const;

	void SetIsBeingThrownByPlayer(bool isBeingThrown) { _isBeingThrownByPlayer = isBeingThrown; }

	const FGameplayTag& GetResourceType() const { return _resourceType; }

	bool AttemptMoveToActor(AActor* actor, UResourceInventory* actorInventory, const FVector& customEndLocation = FVector::ZeroVector);

	static FOnCollected OnCollected;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void OnThrownOnGround();
	virtual void EnableAllCollision(bool enableCollision);

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnGroundHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnHitGround();

	void OnPlayerPickupPreventionTimerEnd();

	void StartMovingTowardsActor();

	UFUNCTION()
	void OnDayEnd();

	void AddResourcesToInventory(UResourceInventory* inventory);

	bool _isBeingThrownByPlayer = false;

	bool _isMovingToActor = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float _rotationRate = 0.f;

	float _rotationVariance = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float _bounceRate = 0.f;

	float _bounceVariance = 1.f;

	float _startingHeight = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float _timeCannotMoveToPlayerAfterSpawn = .5f;

	bool _isPlayerPickupPreventionTimeOver = false;

	UPROPERTY(EditDefaultsOnly)
	float _timeToMoveToActor = 1.5f;

	float _timeMovedToActor = 0.f;

	FTimerHandle _pickupPreventionTimerHandle;

	FVector _startingMovementLocation = FVector::ZeroVector;
	FVector _customEndLocation = FVector::ZeroVector;
	TWeakObjectPtr<AActor> _actorToMoveTo;
	TWeakObjectPtr<UResourceInventory> _inventoryOfActorMovingTowards;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* _capsuleCollider = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* _staticMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Resource", meta = (Categories = "ResourceType."))
	FGameplayTag _resourceType;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	uint16 _resourceAmount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onCollectResourceSound = nullptr;
};
