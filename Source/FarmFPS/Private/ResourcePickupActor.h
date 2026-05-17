// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PoolableActor.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

// Generated
#include "ResourcePickupActor.generated.h"

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

	void SetIsBeingThrownByPlayer(bool isBeingThrown) { _isBeingThrownByPlayer = isBeingThrown; }

	const FGameplayTag& GetResourceType() const { return _cropType; }

	bool AttemptMoveToActor(AActor* actor, UResourceInventory* actorInventory, const FVector& customEndLocation = FVector::ZeroVector);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	bool CanBeCollectedByPlayer() const;
	virtual void OnThrownOnGround();

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGroundHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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
	USphereComponent* _playerCollider = nullptr;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* _capsuleCollider = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* _staticMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield", meta = (Categories = "ResourceType."))
	FGameplayTag _cropType;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Yield")
	uint16 _yieldAmount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> _onCollectResourceSound = nullptr;
};
