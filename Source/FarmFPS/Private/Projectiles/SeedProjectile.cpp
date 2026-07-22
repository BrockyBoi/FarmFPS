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
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASeedProjectile::ASeedProjectile() : Super()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASeedProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->IgnoreActorWhenMoving(FarmFPSUtilities::GetPlayerCharacter(this), true);
}

void ASeedProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* playerCharacter = FarmFPSUtilities::GetPlayerCharacter(this);
	ASeedProjectile* otherProjectile = Cast<ASeedProjectile>(Other);
	if (Other == playerCharacter || IsValid(otherProjectile))
	{
		return;
	}

	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(GetWorld())) && ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);

		UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
		UFarmingPlotComponent* farmPlot = Other->FindComponentByClass<UFarmingPlotComponent>();
		if (IsValid(farmPlot) && farmPlot->GetAllowedSeedTypes().HasTag(ProjectileType) && IsValid(dayNightCycle) && dayNightCycle->IsDay())
		{
			ACrop* crop = Cast<ACrop>(actorPool->GetActorFromPool(ProjectileType, HitLocation, EPooledActorType::Crop));
			if (ensure(IsValid(crop)))
			{
				FVector modifiedVector = HitLocation;

				// Make sure that there are no other crops at current location
				if (!CanSpawnCropAtLocation(crop, modifiedVector, HitNormal))
				{
					int maxAttemptsAllowed = 24;
					int currentAttempt = 0;
					float distFromHit = _minDistanceFromNearestCrop.GetModifiedValue(this);
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
					} while (!CanSpawnCropAtLocation(crop, modifiedVector, HitNormal) && currentAttempt < maxAttemptsAllowed);
				}

				if (CanSpawnCropAtLocation(crop, modifiedVector, HitNormal))
				{
					crop->SetActorLocation(modifiedVector);
					crop->SetActorRotation(Other->GetActorRotation());
				}
				else
				{
					actorPool->AddActorToPool(ProjectileType, crop, EPooledActorType::Crop);
					return;
				}
			}

			if (IsValid(_onSeedPlantedSound))
			{
				UGameplayStatics::SpawnSoundAtLocation(this, _onSeedPlantedSound, GetActorLocation());
			}
		}
	}
}

bool ASeedProjectile::CanSpawnCropAtLocation(ACrop* crop, const FVector& spawnLocation, const FVector& hitNormal) const
{
	bool hasOtherCrops = false;
	bool hasValidFarmPlot = false;
	if (ensure(IsValid(crop)) && ensure(IsValid(crop->GetCapsuleComponent())) && ensure(IsValid(GetWorld())))
	{
		// Check that we don't hit any other crops
		TArray<FHitResult> outHits;
		FCollisionQueryParams params;
		params.AddIgnoredActor(crop);
		FRotator slopeRotator = UKismetMathLibrary::MakeRotFromZ(hitNormal);

		auto shape = crop->GetCapsuleComponent()->GetCollisionShape();
		GetWorld()->SweepMultiByChannel(outHits, spawnLocation, spawnLocation, FQuat(slopeRotator), _collisionChannelToCheck, crop->GetCapsuleComponent()->GetCollisionShape(), params);
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
			GetWorld()->LineTraceMultiByChannel(outHits, spawnLocation + (hitNormal * 10.f), spawnLocation - (hitNormal * 20.f), _collisionChannelToCheck, params);
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
	}

	return hasValidFarmPlot && !hasOtherCrops;
}
