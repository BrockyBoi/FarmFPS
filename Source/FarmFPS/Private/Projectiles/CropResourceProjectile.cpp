// Fill out your copyright notice in the Description page of Project Settings.

#include "CropResourceProjectile.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/FarmFPSUtilities.h"

#include "Misc/MoonHitBox.h"

#include "Plants/Crop.h"
#include "Plants/Plant.h"

#include "StatusEffects/ArcResourceToOtherCropOnHit.h"
#include "StatusEffects/LingeringStatusEffectOnHit.h"

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
}

void ACropResourceProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACropResourceProjectile::EnableCropCollider(const bool enable)
{
	if (ensure(IsValid(_cropCollider)))
	{
		if (enable)
		{
			_cropCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			
			_cropCollider->OnComponentBeginOverlap.RemoveAll(this);
			_cropCollider->OnComponentBeginOverlap.AddDynamic(this, &ACropResourceProjectile::OnComponentOverlap);
		}
		else
		{
			_cropCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			_cropCollider->OnComponentBeginOverlap.RemoveAll(this);
		}
	}
}

void ACropResourceProjectile::EnablePhysicsCollider(const bool enable)
{
	if (ensure(IsValid(CollisionComponent)))
	{
		if (enable)
		{
			CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		}
		else
		{
			CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ACropResourceProjectile::InteractWithPlant(APlant* plant)
{
	if (ensure(IsValid(plant)))
	{
		UResourceInventory* resourceInventory = plant->GetResourceInventory();
		if (ensure(IsValid(resourceInventory)))
		{
			if (_canGiveResources && resourceInventory->CanAddResource(ProjectileType))
			{
				plant->AddResource(ProjectileType, _currentResourceAmount);
				_currentResourceAmount *= (1 - _resourceDecayOnHit.GetModifiedValue(this));
			}
		}

		if (_currentResourceAmount <= 0)
		{
			UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
			if (ensure(IsValid(actorPool)))
			{
				actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
				return;
			}
		}

		ACrop* crop = Cast<ACrop>(plant);
		if (IsValid(crop))
		{
			float procValue = FMath::RandRange(0.f, 1.f);
			if (procValue >= 1 - _addResourceOverTimeEffectData.PercentageToProcEffect.GetModifiedValue(this))
			{
				LingeringStatusEffectOnHit* lingeringEffect = new LingeringStatusEffectOnHit(ProjectileType, crop, _addResourceOverTimeEffectData.LingerDuration.GetModifiedValue(this), _addResourceOverTimeEffectData.ResourcesPerSecond.GetModifiedValue(this));
				if (ensure(lingeringEffect != nullptr))
				{
					lingeringEffect->StartEffect();
				}
			}

			if (procValue >= 1 - _arcBetweenCropsEffectData.PercentageToProcEffect.GetModifiedValue(this))
			{
				ArcResourceToOtherCropOnHit* arcEffect = new ArcResourceToOtherCropOnHit(ProjectileType, crop, _currentResourceAmount, _arcBetweenCropsEffectData.MaxArcCount.GetModifiedValue(this), _arcBetweenCropsEffectData.PercentageToProcEffect.GetModifiedValue(this), _cropCollisionChannel);
				if (ensure(arcEffect != nullptr))
				{
					arcEffect->StartEffect();
				}
			}
		}
	}
}

void ACropResourceProjectile::AddActorToPool()
{
	Super::AddActorToPool();

	EnableCropCollider(false);
	EnablePhysicsCollider(false);
}

void ACropResourceProjectile::RemoveFromPool()
{
	Super::RemoveFromPool();

	EnableCropCollider(_enableCropColliderOnRemoveFromPool);
	EnablePhysicsCollider(true);

	CollisionComponent->IgnoreActorWhenMoving(FarmFPSUtilities::GetPlayerCharacter(this), true);

	_currentResourceAmount = _resourceAmount.GetModifiedValue(this);
}

void ACropResourceProjectile::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ensure(IsValid(OtherActor)))
	{
		APlant* plant = Cast<APlant>(OtherActor);
		if (IsValid(plant))
		{
			InteractWithPlant(plant);
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
