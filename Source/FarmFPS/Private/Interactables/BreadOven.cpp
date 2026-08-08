// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadOven.h"

// Brock
#include "Projectiles/CropResourceProjectile.h"
#include "Resources/ResourceTypeTags.h"

ABreadOven::ABreadOven() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	_fireTargetCollider = CreateDefaultSubobject<USphereComponent>("Fire Target Collider");
	_fireTargetCollider->SetupAttachment(RootComponent);
}

float ABreadOven::GetHeatMultipler() const
{
	if (ShouldGainHeatMultiplier())
	{
		return _idealHeatSpeedModifier.GetModifiedValue(this);
	}
	return 1.0f;
}

bool ABreadOven::ShouldGainHeatMultiplier() const
{
	return _ovenHeat >= _idealHeatMin && _ovenHeat <= _idealHeatMax;
}

bool ABreadOven::IsHeatTooHigh() const
{
	return _ovenHeat >= _heatToDestroyBread;
}

void ABreadOven::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_fireTargetCollider)))
	{
		_fireTargetCollider->OnComponentBeginOverlap.AddDynamic(this, &ABreadOven::OnFireTargetOverlap);
	}
}

void ABreadOven::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHeatLevel(_ovenHeat - (_heatLostPerSecond * DeltaTime));
}

void ABreadOven::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_fireTargetCollider))
	{
		_fireTargetCollider->OnComponentBeginOverlap.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ABreadOven::OnFireTargetOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACropResourceProjectile* cropProjectile = Cast<ACropResourceProjectile>(OtherActor);
	if (IsValid(cropProjectile))
	{
		if (cropProjectile->GetProjectileType() == ResourceTypeTags::Light)
		{
			SetHeatLevel(_ovenHeat + _heatGainOnLight);
		}
		else if (cropProjectile->GetProjectileType() == ResourceTypeTags::Water)
		{
			SetHeatLevel(_ovenHeat - _heatLossOnWater);
		}
	}
}

float ABreadOven::GetTimeBetweenSpawns() const
{
	return Super::GetTimeBetweenSpawns() * GetHeatMultipler();
}

void ABreadOven::SpawnResource(ResourcesToSpawnData& data)
{
	if (IsHeatTooHigh())
	{
		data.AmountToSpawn--;
		return;
	}

	Super::SpawnResource(data);
}

FLinearColor ABreadOven::GetHeatColorText() const
{
	if (_ovenHeat < _idealHeatMin)
	{
		return FMath::Lerp(FLinearColor::Blue, FLinearColor::Green, _ovenHeat / _idealHeatMin);
	}
	else if (_ovenHeat >= _idealHeatMin && _ovenHeat <= _idealHeatMax)
	{
		return FLinearColor::Green;
	}
	else if (_ovenHeat > _idealHeatMax && _ovenHeat < _heatToDestroyBread)
	{
		return FMath::Lerp(FLinearColor::Green, FLinearColor::Yellow, (_ovenHeat - _idealHeatMax) / (_heatToDestroyBread - _idealHeatMax));
	}
	else if (_ovenHeat >= _heatToDestroyBread)
	{
		return FLinearColor::Red;
	}
	return FLinearColor();
}

void ABreadOven::SetHeatLevel(float newHeat)
{
	_ovenHeat = FMath::Clamp(newHeat, 0.f, 100.f);
	OnOvenHeatChanged.Broadcast(_ovenHeat);
	Cosmetic_OnOvenHeatChanged(_ovenHeat);
}
