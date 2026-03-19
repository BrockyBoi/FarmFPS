// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ShooterProjectile.h"

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> _cropActorClass;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."))
	FGameplayTag _seedType;
};
