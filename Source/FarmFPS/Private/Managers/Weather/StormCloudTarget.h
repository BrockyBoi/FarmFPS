// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "StormCloudTarget.generated.h"

UCLASS()
class AStormCloudTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	AStormCloudTarget();

	void SetDirection(const FVector& NewDirection) { _direction = NewDirection; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void Cosmetic_OnStormCloudHit(const FGameplayTag& resourceTag);

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector _direction = FVector::ZeroVector;
	bool _wasShot = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float _speed = 100.0f;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* _staticMesh = nullptr;
};
