// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeActor.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/FarmFPSUtilities.h"

AGrenadeActor::AGrenadeActor() : Super()
{
	_explosionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollider"));
	_explosionCollider->SetupAttachment(RootComponent);
	_explosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_explosionCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AGrenadeActor::AddActorToPool()
{
	Super::AddActorToPool();

	if (ensure(IsValid(_explosionCollider)))
	{
		_explosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGrenadeActor::RemoveFromPool()
{
	Super::RemoveFromPool();

	if (ensure(IsValid(_explosionCollider)))
	{
		_explosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGrenadeActor::BeginPlay()
{
	Super::BeginPlay();
}

void AGrenadeActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGrenadeActor::OnThrownOnGround()
{
	Super::OnThrownOnGround();

	if (ensure(IsValid(_explosionCollider)))
	{
		_explosionCollider->SetSphereRadius(_explosionRadius.GetModifiedValue(this));
		_explosionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		GetWorldTimerManager().SetTimer(_explosionTimerHandle, this, &AGrenadeActor::OnExplosionTimerFinished, _explosionDuration);

		TSet<AActor*> overlappingActors;
		_explosionCollider->GetOverlappingActors(overlappingActors);

		for (AActor* overlappingActor : overlappingActors)
		{
			if (IsValid(overlappingActor))
			{
				ExplosionAffectActor(overlappingActor);
			}
		}
	}
}

void AGrenadeActor::ExplosionAffectActor(AActor* actor)
{
	if (ensure(IsValid(actor)) && _appliesPhysicalForce)
	{
		AResourcePickupActor* resourceActor = Cast<AResourcePickupActor>(actor);
		if (IsValid(resourceActor))
		{
			FVector explosionToActor = actor->GetActorLocation() - GetActorLocation();
			explosionToActor.Normalize();
			FVector explosionVector = explosionToActor * _explosionForce.GetModifiedValue(this);

			UPrimitiveComponent* primitiveComponent = actor->FindComponentByClass<UPrimitiveComponent>();
			if (IsValid(primitiveComponent))
			{
				primitiveComponent->AddImpulse(explosionVector, NAME_None, true);
			}
		}
	}
}

void AGrenadeActor::OnExplosionTimerFinished()
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (IsValid(actorPool))
	{
		actorPool->AddActorToPool(_resourceType, this, EPooledActorType::ResourcePickup);
	}
}

