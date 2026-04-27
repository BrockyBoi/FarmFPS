// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
class AResourcePickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourcePickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnCapsuleColliderHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnPickupPreventionTimerEnd();

	void StartMovingTowardsPlayer();

	UFUNCTION()
	void OnDayEnd();

	void AddResourcesToPlayerInventory(UResourceInventory* inventory);

	bool _isMovingToPlayer = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float _rotationRate = 0.f;

	float _rotationVariance = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float _bounceRate = 0.f;

	float _bounceVariance = 1.f;

	float _startingHeight = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float _timeCannotMoveToPlayerAfterSpawn = .5f;

	bool _isAllowedToBeCollected = false;

	UPROPERTY(EditDefaultsOnly)
	float _timeToMoveToPlayer = 1.5f;

	float _timeMovedToPlayer = 0.f;

	FTimerHandle _pickupPreventionTimerHandle;


	FVector _startingMovementLocation = FVector::ZeroVector;
	TWeakObjectPtr<APawn> _characterToMoveTo;

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
};
