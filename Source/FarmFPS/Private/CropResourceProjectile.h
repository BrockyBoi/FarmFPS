// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere)
	USphereComponent* _cropCollider = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crop Resource", meta = (Categories = "CropResourceType."))
	FGameplayTag _cropResourceType;

	UPROPERTY(EditDefaultsOnly, Category = "Crop Resource")
	FModifiedFloatValue _resourceAmount = 1.f;
};
