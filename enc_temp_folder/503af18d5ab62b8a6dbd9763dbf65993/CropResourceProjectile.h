// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "StatusEffects/EffectManager.h"

// Shooter
#include "ShooterProjectile.h"

// UE
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"

// Generated
#include "CropResourceProjectile.generated.h"

UCLASS()
class ACropResourceProjectile : public AShooterProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACropResourceProjectile();
	float GetCropResourceAmount(const UObject* worldObject) const { return _currentResourceAmount; }

	virtual void AddActorToPool() override;
	virtual void RemoveFromPool() override;
	virtual void ModifyThroughChargingWeapon(float multiplier) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void EnableCropCollider(const bool enable);
	void EnablePhysicsCollider(const bool enable);

	void InteractWithPlant(class APlant* plant);

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere)
	USphereComponent* _cropCollider = nullptr;

	bool _canGiveResources = true;

	float _currentResourceAmount = 0;

	UPROPERTY(EditDefaultsOnly)
	bool _enableCropColliderOnRemoveFromPool = true;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Resource")
	FModifiedFloatValue _resourceAmount = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Resource")
	FModifiedFloatValue _resourceDecayOnHit = .1f;

	UPROPERTY(EditDefaultsOnly, Category = "Lingering Status Effect")
	FAddResourceOverTimeEffectData _addResourceOverTimeEffectData;

	UPROPERTY(EditDefaultsOnly, Category = "Arc Status Effect")
	FArcBetweenCropsEffectData _arcBetweenCropsEffectData;

	UPROPERTY(EditAnywhere, Category = "Charging")
	float ResourceMultiplierAtMaxCharge = 3.f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> _cropCollisionChannel;
};
