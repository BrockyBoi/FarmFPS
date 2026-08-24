// Fill out your copyright notice in the Description page of Project Settings.

#include "WindAffector.h"

// Brock
#include "FarmFPSCharacter.h"

// UE
#include "GameFramework/ProjectileMovementComponent.h"

UWindAffector::UWindAffector()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWindAffector::BeginPlay()
{
	Super::BeginPlay();

	_playerCollider = GetOwner()->FindComponentByClass<UShapeComponent>();
	if (ensure(IsValid(_playerCollider)))
	{
		_playerCollider->OnComponentBeginOverlap.AddDynamic(this, &UWindAffector::OnComponentOverlap);
		_playerCollider->OnComponentEndOverlap.AddDynamic(this, &UWindAffector::OnComponentOverlapEnd);
	}

	OnWindDirectionChangeTimerEnd();
	GetWorld()->GetTimerManager().SetTimer(_timerHandle, this, &UWindAffector::OnWindDirectionChangeTimerEnd, FMath::RandRange(_minWindDirectionDuration, _maxWindDirectionDuration), false);
}

void UWindAffector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector2D finalVec = _currentWindDirection * _windStrength.GetModifiedValue(this) * DeltaTime;
	const FVector finalVec3D = FVector(finalVec.X, finalVec.Y, 0);
	if (_playerInCollider.IsValid())
	{
		_playerInCollider->AddForeignMovement(finalVec);
	}

	for (TWeakObjectPtr<UPrimitiveComponent> primitiveComponent : _primitiveComponents)
	{
		if (ensure(primitiveComponent.IsValid()))
		{
			primitiveComponent->AddForce(finalVec3D * _resourceActorStrengthBonus);
		}
	}

	for (TWeakObjectPtr<UProjectileMovementComponent> projectile : _projectiles)
	{
		if (ensure(projectile.IsValid()))
		{
			projectile->Velocity += finalVec3D * _projectileStrengthBonus;
		}
	}
}

void UWindAffector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_playerCollider))
	{
		_playerCollider->OnComponentBeginOverlap.RemoveAll(this);
		_playerCollider->OnComponentEndOverlap.RemoveAll(this);
	}

	_projectiles.Empty();
	_primitiveComponents.Empty();
	_playerInCollider = nullptr;

	Super::EndPlay(EndPlayReason);
}

void UWindAffector::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(OtherActor);
	if (IsValid(player))
	{
		_playerInCollider = player;
	}
	else
	{
		UProjectileMovementComponent* projectileMovement = OtherActor->FindComponentByClass<UProjectileMovementComponent>();
		if (IsValid(projectileMovement))
		{
			_projectiles.Add(projectileMovement);
		}
		else
		{
			_primitiveComponents.Add(OtherComp);
		}
	}
}

void UWindAffector::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(OtherActor);
	if (IsValid(player) && player == _playerInCollider)
	{
		_playerInCollider = nullptr;
	}
	else
	{
		UProjectileMovementComponent* projectileMovement = OtherActor->FindComponentByClass<UProjectileMovementComponent>();
		if (IsValid(projectileMovement))
		{
			_projectiles.Remove(projectileMovement);
		}
		else
		{
			_primitiveComponents.Remove(OtherComp);
		}
	}
}

void UWindAffector::OnWindDirectionChangeTimerEnd()
{
	_currentWindDirection = FMath::RandPointInCircle(1);
	_currentWindDirection.Normalize();
	GetWorld()->GetTimerManager().SetTimer(_timerHandle, this, &UWindAffector::OnWindDirectionChangeTimerEnd, FMath::RandRange(_minWindDirectionDuration, _maxWindDirectionDuration), false);

	OnWindDirectionChanged.Broadcast();
}

