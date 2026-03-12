// Fill out your copyright notice in the Description page of Project Settings.

#include "CropComponent.h"

// Brock
#include "FarmFPSUtilities.h"
#include "PerkManager.h"
#include "PerkModifierTypeTags.h"
#include "ResourcePickupActor.h"

UCropComponent::UCropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCropComponent::BeginPlay()
{
	Super::BeginPlay();

	AffectGrowth();
}

void UCropComponent::AddWater(float waterAmount)
{
	_currentWaterLevel = FMath::Clamp(_currentWaterLevel + waterAmount, 0.f, _cropData.WaterNeeded);
	AffectGrowth();
}

void UCropComponent::AddLight(float lightAmount)
{
	_currentLightLevel = FMath::Clamp(_currentLightLevel + lightAmount, 0.f, _cropData.LightNeeded);
	AffectGrowth();
}

void UCropComponent::BreakCrop()
{
	if (ensure(IsValid(GetWorld())) && ensure(IsValid(GetOwner())) && ensure(IsValid(_cropYieldPickupClass)))
	{
		UPerkManager* perkManager = FarmFPSUtilities::GetPlayerPerkManager(this);
		if (!ensure(IsValid(perkManager)))
		{
			return;
		}

		int countToDrop = perkManager->ModifyValueByPerks(PerkModifierTypeTags::MoreCropYield, _cropData.NumberOfPickupsToDrop);
		for (int i = 0; i < countToDrop; i++)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AResourcePickupActor* pickup = GetWorld()->SpawnActor<AResourcePickupActor>(_cropYieldPickupClass, GetOwner()->GetActorLocation() + FVector::UpVector * _yieldPickupSpawnHeight, FRotator::ZeroRotator, SpawnParams);
			
			// Pickup may not be valid if immediately collected by player
			if (IsValid(pickup))
			{
				UPrimitiveComponent* pickupCollider = pickup->FindComponentByClass<UPrimitiveComponent>();
				if (IsValid(pickupCollider))
				{
					pickupCollider->AddImpulse(FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 200.f), NAME_None, true);
				}
			}
		}

		GetOwner()->Destroy();
	}
}

void UCropComponent::AffectGrowth()
{
	if (ensure(IsValid(GetOwner())))
	{
		float waterGrowthRatio = _currentWaterLevel / _cropData.WaterNeeded;
		float lightGrowthRatio = _currentLightLevel / _cropData.LightNeeded;
		float scaleAmount = FMath::Lerp(_cropData.StartingScaleSize, _cropData.FinalScaleSize, (waterGrowthRatio + lightGrowthRatio) / 2);

		UStaticMeshComponent* cropMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		if (ensure(IsValid(cropMesh)))
		{
			cropMesh->SetWorldScale3D(FVector::One() * scaleAmount);
		}

		if (_currentLightLevel >= _cropData.LightNeeded && _currentWaterLevel >= _cropData.WaterNeeded)
		{
			BreakCrop();
		}
	}
}

