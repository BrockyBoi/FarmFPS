// Fill out your copyright notice in the Description page of Project Settings.

#include "RocketProjectile.h"

// Brock
#include "FarmFPSCharacter.h"
#include "Plants/Plant.h"

// UE
#include "GameFramework/CharacterMovementComponent.h"

ARocketProjectile::ARocketProjectile() : Super()
{
	_enableColliderOnRemoveFromPool = false;
}

void ARocketProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();

	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void ARocketProjectile::RemoveFromPool()
{
	Super::RemoveFromPool();
}

void ARocketProjectile::Explode()
{
	EnableCollider(true);
	_cropCollider->SetSphereRadius(_explosionRange.GetModifiedValue(this));

	TSet<AActor*> overlappingActors;
	_cropCollider->GetOverlappingActors(overlappingActors);

	for (AActor* actor : overlappingActors)
	{
		if (IsValid(actor))
		{
			APlant* plant = Cast<APlant>(actor);
			if (IsValid(plant))
			{
				InteractWithPlant(plant);
			}

			FVector explosionToActor =  actor->GetActorLocation() - GetActorLocation();
			explosionToActor.Normalize();
			FVector explosionVector = explosionToActor * _explosionForce.GetModifiedValue(this);

			UPrimitiveComponent* primitiveComponent = actor->FindComponentByClass<UPrimitiveComponent>();
			if (IsValid(primitiveComponent))
			{
				primitiveComponent->AddForce(explosionVector);
			}

			AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(actor);
			if (IsValid(player))
			{
				player->LaunchCharacter(explosionVector, false, false);
			}
		}
	}
}
