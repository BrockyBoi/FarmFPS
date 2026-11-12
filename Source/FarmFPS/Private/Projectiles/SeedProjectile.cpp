// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedProjectile.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/AudioManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Misc/SoftObjectHelper.h"
#include "Plants/Crop.h"
#include "Plants/FarmingPlotComponent.h"
#include "Resources/ResourcePickupActor.h"
#include "ShooterWeapon.h"

// UE
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASeedProjectile::ASeedProjectile() : Super()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASeedProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->IgnoreActorWhenMoving(UFarmFPSUtilities::GetPlayerCharacter(this), true);
	
	if (UCropProjectileDataAsset* projData = Cast<UCropProjectileDataAsset>(_projectileDataAsset); ensure(IsValid(projData)))
	{
		SoftObjectHelper::AsyncLoadSoftObjectNoCallback(this, projData->OnFailToPlantSeedSound);
		SoftObjectHelper::AsyncLoadSoftObjectNoCallback(this, projData->OnPlantSeedSound);
	}

	if (UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this); ensure(IsValid(actorPool)))
	{
		actorPool->InitializeActors(ProjectileType, 1, EPooledActorType::Crop);
	}
}

void ASeedProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* playerCharacter = UFarmFPSUtilities::GetPlayerCharacter(this);
	ASeedProjectile* otherProjectile = Cast<ASeedProjectile>(Other);
	if (Other == playerCharacter || IsValid(otherProjectile))
	{
		return;
	}

	UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(GetWorld())) && ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);

		UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
		UFarmingPlotComponent* farmPlot = Other->FindComponentByClass<UFarmingPlotComponent>();
		if (IsValid(farmPlot) && farmPlot->GetAllowedSeedTypes().HasTag(ProjectileType) && IsValid(dayNightCycle) && dayNightCycle->IsDay())
		{
			FCollisionShape shape = FCollisionShape();
			shape.SetCapsule(_capsuleHeight, _capsuleWidth);

			FVector modifiedVector = HitLocation;

			// Make sure that there are no other crops at current location
			if (!CanSpawnCropAtLocation(shape, modifiedVector, HitNormal))
			{
				int maxAttemptsAllowed = 24;
				int currentAttempt = 0;
				float distFromHit = _minDistanceFromNearestCrop.GetValue().GetModifiedValue(this);
				FVector randVector = FVector::Zero();
				FRotator slopeRotator = UKismetMathLibrary::MakeRotFromZ(HitNormal);
				FVector slopeForward = FRotationMatrix(slopeRotator).GetUnitAxis(EAxis::X);
				FVector slopeRight = FRotationMatrix(slopeRotator).GetUnitAxis(EAxis::Y);
				float angle = FMath::RandRange(0, 360);

				do
				{
					currentAttempt++;
					// Add an extra 15 degrees each attempt to find valid spot
					angle += currentAttempt * 15;

					float degToRad = FMath::DegreesToRadians(angle);
					float scaleX = distFromHit * FMath::Cos(angle);
					float scaleY = distFromHit * FMath::Sin(angle);

					modifiedVector = HitLocation + (slopeForward * scaleX) + (slopeRight * scaleY);
				} while (!CanSpawnCropAtLocation(shape, modifiedVector, HitNormal) && currentAttempt < maxAttemptsAllowed);
			}

			if (CanSpawnCropAtLocation(shape, modifiedVector, HitNormal))
			{
				if (ACrop* crop = Cast<ACrop>(actorPool->GetActorFromPool(ProjectileType, HitLocation, EPooledActorType::Crop)); ensure(IsValid(crop)))
				{
					crop->SetActorLocation(modifiedVector);
					crop->SetActorRotation(Other->GetActorRotation());

					crop->SetWasPlantedSuccessfully(true);
					OnSpawnSeedSuccess();
				}
			}
			else
			{
				UCropProjectileDataAsset* projData = Cast<UCropProjectileDataAsset>(_projectileDataAsset);
				if (ensure(IsValid(projData)) && !projData->OnFailToPlantSeedSound.IsNull() && ensure(_owningWeapon.IsValid()) && _owningWeapon->GetWeaponFireType() == EWeaponFireType::SemiAuto)
				{
					SoftObjectHelper::PlaySpawnedSoundAtLocation(this, projData->OnFailToPlantSeedSound, GetActorLocation());
				}

				OnSpawnSeedFail();
				return;
			}
		}

		UCropProjectileDataAsset* projData = Cast<UCropProjectileDataAsset>(_projectileDataAsset);
		if (ensure(IsValid(projData)) && !projData->OnPlantSeedSound.IsNull())
		{
			SoftObjectHelper::PlaySpawnedSoundAtLocation(this, projData->OnPlantSeedSound, GetActorLocation());
		}
	}
	else
	{
		OnSpawnSeedFail();
	}
}

bool ASeedProjectile::CanSpawnCropAtLocation(const FCollisionShape& collisionShape, const FVector& spawnLocation, const FVector& hitNormal) const
{
	UCropProjectileDataAsset* projData = Cast<UCropProjectileDataAsset>(_projectileDataAsset);
	if (!ensure(IsValid(projData)) || !ensure(IsValid(GetWorld())))
	{
		return false;
	}

	bool hasOtherCrops = false;
	bool hasValidFarmPlot = false;
	// Check that we don't hit any other crops
	TArray<FHitResult> outHits;
	FCollisionQueryParams params;
	FRotator slopeRotator = UKismetMathLibrary::MakeRotFromZ(hitNormal);

	GetWorld()->SweepMultiByChannel(outHits, spawnLocation, spawnLocation, FQuat(slopeRotator), projData->CollisionChannelToCheck, FCollisionShape::MakeCapsule(collisionShape.GetCapsuleRadius() / 2.f, collisionShape.GetCapsuleHalfHeight()), params);
	for (FHitResult& result : outHits)
	{
		AActor* actor = result.GetActor();
		if (IsValid(actor))
		{
			if (actor->IsA<ACrop>())
			{
				hasOtherCrops = true;
				break;
			}
			//else if (IsValid(actor->FindComponentByClass<UFarmingPlotComponent>()) && actor->FindComponentByClass<UFarmingPlotComponent>()->GetAllowedSeedTypes().HasTag(ProjectileType))
			//{
			//	hasValidFarmPlot = true;
			//}
		}
	}

	// Double check that new location is actually on top of farm plot
	if (!hasOtherCrops)
	{
		GetWorld()->LineTraceMultiByChannel(outHits, spawnLocation + (hitNormal * 10.f), spawnLocation - (hitNormal * 20.f), projData->CollisionChannelToCheck, params);
		for (FHitResult& result : outHits)
		{
			AActor* actor = result.GetActor();
			if (IsValid(actor))
			{
				UFarmingPlotComponent* farmPlot = actor->GetComponentByClass<UFarmingPlotComponent>();
				if (IsValid(farmPlot) && farmPlot->GetAllowedSeedTypes().HasTag(ProjectileType))
				{
					return true;
				}
			}
		}
	}

	return hasValidFarmPlot && !hasOtherCrops;
}

void ASeedProjectile::OnSpawnSeedFail()
{
	Cosmetic_OnSpawnSeedFail();

	SpawnAsyncOnProjectileHitVFX(_failedSpawnSeedVFXScale);
}

void ASeedProjectile::OnSpawnSeedSuccess()
{
	Cosmetic_OnSpawnedSeedSucceed();

	SpawnAsyncOnProjectileHitVFX(_succeedSpawnSeedVFXScale);
}
