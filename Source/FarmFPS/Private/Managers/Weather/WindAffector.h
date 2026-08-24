// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"

// Generated
#include "WindAffector.generated.h"

class AFarmFPSCharacter;
class UProjectileMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWindAffector : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWindAffector();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindDirectionChanged);

	UPROPERTY(BlueprintAssignable)
	FOnWindDirectionChanged OnWindDirectionChanged;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnWindDirectionChangeTimerEnd();

	TWeakObjectPtr<AFarmFPSCharacter> _playerInCollider = nullptr;

	FVector2D _currentWindDirection = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	float _minWindDirectionDuration = 3.f;

	UPROPERTY(EditAnywhere)
	float _maxWindDirectionDuration = 10.f;

	UPROPERTY(EditAnywhere)
	FModifiedFloatValue _windStrength = .5f;

	UPROPERTY(EditAnywhere)
	float _resourceActorStrengthBonus = 1000;

	UPROPERTY(EditAnywhere)
	float _projectileStrengthBonus = 1000;

	UShapeComponent* _playerCollider = nullptr;

	TArray<TWeakObjectPtr<UPrimitiveComponent>> _primitiveComponents;
	TArray<TWeakObjectPtr<UProjectileMovementComponent>> _projectiles;

	FTimerHandle _timerHandle;
};
