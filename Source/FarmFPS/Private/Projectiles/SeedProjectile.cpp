// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedProjectile.h"

// Brock
#include "Managers/ActorPool.h"
#include "Plants/Crop.h"
#include "Managers/DayNightCycleManager.h"
#include "Plants/FarmingPlotComponent.h"
#include "Managers/FarmFPSUtilities.h"
#include "Resources/ResourcePickupActor.h"

// UE
#include "Kismet/GameplayStatics.h"

// Sets default values
ASeedProjectile::ASeedProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASeedProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASeedProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(GetWorld())) && ensure(IsValid(actorPool)))
	{
		UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
		UFarmingPlotComponent* farmPlot = Other->FindComponentByClass<UFarmingPlotComponent>();
		if (IsValid(farmPlot) && farmPlot->GetAllowedSeedTypes().HasTag(ProjectileType) && IsValid(dayNightCycle) && dayNightCycle->IsDay())
		{
			ACrop* crop = Cast<ACrop>(actorPool->GetActorFromPool(ProjectileType, HitLocation, EPooledActorType::Crop));
			if (ensure(IsValid(crop)))
			{
				// Make sure that there are no other crops at current location
				if (!CanSpawnCropAtLocation(crop, HitLocation))
				{
					int maxAttemptsAllowed = 25;
					int currentAttempt = 0;
					float distFromHit = _minDistanceFromNearestCrop.GetModifiedValue(this);
					FVector randVector = FVector::Zero();
					FVector modifiedVector = FVector::Zero();
					do
					{
						FVector2D vec = FMath::RandPointInCircle(distFromHit);
						randVector = FVector(vec.X, vec.Y, HitLocation.Z);
						randVector.Normalize();
						randVector *= distFromHit;
						currentAttempt++;
						modifiedVector = HitLocation + randVector;
					} while (!CanSpawnCropAtLocation(crop, modifiedVector) && currentAttempt < maxAttemptsAllowed);

					if (CanSpawnCropAtLocation(crop, modifiedVector))
					{
						crop->SetActorLocation(modifiedVector);
					}
				}
			}

			if (ensure(IsValid(_onSeedPlantedSound)))
			{
				UGameplayStatics::SpawnSoundAtLocation(this, _onSeedPlantedSound, GetActorLocation());
			}
		}

		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
	}
}

bool ASeedProjectile::CanSpawnCropAtLocation(ACrop* crop, const FVector& spawnLocation) const
{
	if (ensure(IsValid(crop)) && ensure(IsValid(GetWorld())))
	{
		FHitResult outHit;
		return !GetWorld()->SweepSingleByChannel(outHit, spawnLocation, spawnLocation, FQuat::Identity, _collisionChannelToCheck, crop->GetCapsuleComponent()->GetCollisionShape());
	}

	return false;
}
