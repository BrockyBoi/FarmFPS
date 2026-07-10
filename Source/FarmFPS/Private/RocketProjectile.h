// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Projectiles/CropResourceProjectile.h"

// UE 
#include "CoreMinimal.h"

// Generated
#include "RocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ARocketProjectile : public ACropResourceProjectile
{
	GENERATED_BODY()

protected:
	ARocketProjectile();
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void RemoveFromPool() override;

private:
	void Explode();

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _explosionRange = 150.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _explosionForce = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float _squaredDistanceForForwardVectorRocketJump = 1000.f;
};
