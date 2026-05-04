// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadOven.h"

// Brock
#include "CropResourceProjectile.h"
#include "ResourceTypeTags.h"

ABreadOven::ABreadOven() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	_fireTargetCollider = CreateDefaultSubobject<USphereComponent>("Fire Target Collider");
	_fireTargetCollider->SetupAttachment(RootComponent);
}

float ABreadOven::GetHeatMultipler() const
{
	if (_ovenHeat >= _idealHeatMin && _ovenHeat <= _idealHeatMax)
	{
		return _idealHeatSpeedModifier.GetModifiedValue(this);
	}
	return 1.0f;
}

void ABreadOven::OnFireTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACropResourceProjectile* cropProjectile = Cast<ACropResourceProjectile>(OtherActor);
	if (IsValid(cropProjectile))
	{
		if (cropProjectile->GetCropResourceType() == ResourceTypeTags::Light)
		{
			_ovenHeat = FMath::Clamp(_ovenHeat + cropProjectile->GetCropResourceAmount(this), 0.f, 100.f);
		}
		else if (cropProjectile->GetCropResourceType() == ResourceTypeTags::Water)
		{
			_ovenHeat = FMath::Clamp(_ovenHeat - cropProjectile->GetCropResourceAmount(this), 0.f, 100.f);
		}
	}
}

float ABreadOven::GetTimeBetweenSpawns() const
{
	return Super::GetTimeBetweenSpawns() * GetHeatMultipler();
}

void ABreadOven::SpawnResource(ResourcesToSpawnData& data)
{
	if (_ovenHeat >= _heatToDestroyBread)
	{
		data.AmountToSpawn--;
		return;
	}

	Super::SpawnResource(data);
}
