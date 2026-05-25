// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplodableResourcePickupActor.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/FarmFPSUtilities.h"
#include "Plants/Plant.h"

// UE
#include "Components/SphereComponent.h"

AExplodableResourcePickupActor::AExplodableResourcePickupActor() : Super()
{
	_explosionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollider"));
	_explosionCollider->SetupAttachment(RootComponent);
	_explosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_explosionCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AExplodableResourcePickupActor::BeginPlay()
{
	Super::BeginPlay();
}

void AExplodableResourcePickupActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AExplodableResourcePickupActor::OnThrownOnGround()
{
	Super::OnThrownOnGround();

	if (ensure(IsValid(_explosionCollider)))
	{
		_explosionCollider->SetSphereRadius(_explosionRadius.GetModifiedValue(this));
		_explosionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		TSet<AActor*> overlappingActors;
		_explosionCollider->GetOverlappingActors(overlappingActors);

		for (AActor* overlappingActor : overlappingActors)
		{
			APlant* plant = Cast<APlant>(overlappingActor);
			if (IsValid(plant))
			{
				OverlapWithPlant(plant);
			}
		}

		GetWorldTimerManager().SetTimer(_explosionTimerHandle, this, &AExplodableResourcePickupActor::OnExplosionTimerFinished, _explosionDuration);
	}
}

void AExplodableResourcePickupActor::AddActorToPool()
{
	Super::AddActorToPool();

	_explosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AExplodableResourcePickupActor::RemoveFromPool()
{
	Super::RemoveFromPool();

	_explosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AExplodableResourcePickupActor::OverlapWithPlant(APlant* plant)
{
	if (ensure(IsValid(plant)))
	{
		plant->AddResource(_explosionResourceType, _explosionResourceValue.GetModifiedValue(this));
	}
}

void AExplodableResourcePickupActor::OnExplosionTimerFinished()
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (IsValid(actorPool))
	{
		actorPool->AddActorToPool(_cropType, this, EPooledActorType::ResourcePickup);
	}
}
