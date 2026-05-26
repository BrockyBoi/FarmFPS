// Fill out your copyright notice in the Description page of Project Settings.

#include "CropResourceProjectile.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/FarmFPSUtilities.h"
#include "Plants/Plant.h"
#include "Misc/MoonHitBox.h"
#include "Resources/ResourceTypeTags.h"

ACropResourceProjectile::ACropResourceProjectile() : Super()
{
	PrimaryActorTick.bCanEverTick = false;

	_cropCollider = CreateDefaultSubobject<USphereComponent>(TEXT("CropCollider"));

	_cropCollider->SetupAttachment(RootComponent);
	_cropCollider->SetSphereRadius(16.0f);
	_cropCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_cropCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}

void ACropResourceProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_cropCollider)))
	{
		_cropCollider->OnComponentBeginOverlap.AddDynamic(this, &ACropResourceProjectile::OnComponentOverlap);
	}
}

void ACropResourceProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_cropCollider))
	{
		_cropCollider->OnComponentBeginOverlap.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ACropResourceProjectile::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ensure(IsValid(OtherActor)))
	{
		APlant* plant = Cast<APlant>(OtherActor);
		if (IsValid(plant))
		{
			plant->AddResource(ProjectileType, _resourceAmount.GetModifiedValue(this));
		}

		UMoonHitBox* moonHitBox = OtherActor->FindComponentByClass<UMoonHitBox>();
		if (ProjectileType == ResourceTypeTags::Light && IsValid(moonHitBox))
		{
			moonHitBox->HitMoon();
		}
	}
}

void ACropResourceProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
	}
}
