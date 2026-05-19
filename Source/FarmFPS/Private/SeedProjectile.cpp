// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedProjectile.h"

// Brock
#include "ActorPool.h"
#include "Crop.h"
#include "DayNightCycleManager.h"
#include "FarmingPlotComponent.h"
#include "FarmFPSUtilities.h"
#include "ResourcePickupActor.h"

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

			}

			if (ensure(IsValid(_onSeedPlantedSound)))
			{
				UGameplayStatics::SpawnSoundAtLocation(this, _onSeedPlantedSound, GetActorLocation());
			}
		}

		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
	}
}
