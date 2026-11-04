// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ShooterProjectile.h"

// Brock
#include "Data/ModifiedValueData.h"
#include "Data/CropProjectileDataAsset.h"
#include "Managers/PoolableActor.h"
#include "Misc/CachedModifiableValueDataTableRowHandle.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

// Generated
#include "SeedProjectile.generated.h"

UCLASS()
class ASeedProjectile : public AShooterProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASeedProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	bool CanSpawnCropAtLocation(const FCollisionShape& collisionShape, const FVector& spawnLocation, const FVector& hitNormal) const;

	void OnSpawnSeedFail();
	void OnSpawnSeedSuccess();

	UPROPERTY(EditDefaultsOnly)
	float _capsuleHeight = 44;

	UPROPERTY(EditDefaultsOnly)
	float _capsuleWidth = 22;

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnSpawnedSeedSucceed();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnSpawnSeedFail();

	UPROPERTY(EditAnywhere)
	float _failedSpawnSeedVFXScale = .2f;

	UPROPERTY(EditAnywhere)
	float _succeedSpawnSeedVFXScale = .5f;

	UPROPERTY(EditDefaultsOnly, Category = "Tunables")
	FCachedModifiableValueDataTableRowHandle _minDistanceFromNearestCrop = 15.f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> _collisionChannelToCheck;
};
