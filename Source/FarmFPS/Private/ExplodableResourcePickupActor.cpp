// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplodableResourcePickupActor.h"

// Brock
#include "ActorPool.h"
#include "Crop.h"
#include "FarmFPSUtilities.h"

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

	_explosionCollider->OnComponentBeginOverlap.AddDynamic(this, &AExplodableResourcePickupActor::OnExplosionOverlap);
}

void AExplodableResourcePickupActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	_explosionCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AExplodableResourcePickupActor::OnExplosionOverlap);

	Super::EndPlay(EndPlayReason);
}

void AExplodableResourcePickupActor::OnThrownOnGround()
{
	if (ensure(IsValid(_explosionCollider)))
	{
		_explosionCollider->SetSphereRadius(_explosionRadius.GetModifiedValue(this));
		_explosionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

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

void AExplodableResourcePickupActor::OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		ACrop* crop = Cast<ACrop>(OtherActor);
		if (IsValid(crop))
		{
			crop->AddCropResourceValue(_explosionResourceType, _explosionResourceValue.GetModifiedValue(this));
		}
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
