// Fill out your copyright notice in the Description page of Project Settings.

#include "Plant.h"

// Brock
#include "Resources/ResourceInventory.h"
#include "Resources/ResourceTypeTags.h"

APlant::APlant()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
}

void APlant::AddResource(const FGameplayTag& resourceType, float amount)
{
	if (_isBroken || amount <= 0.0f)
	{
		return;
	}

	if (ensure(IsValid(_resourcesInventory)))
	{
		if (resourceType == ResourceTypeTags::Damage)
		{
			DoDamage(amount);
		}
		else
		{
			if (!_resourcesInventory->CanAddResource(resourceType, amount) || _isLightAndWaterFull)
			{
				return;
			}

			_resourcesInventory->AddResource(resourceType, amount);

			AffectGrowth();

			if (!_isLightAndWaterFull && IsLightAndWaterFull())
			{
				OnLightAndWaterFilled();
				_isLightAndWaterFull = true;
			}

			Cosmetic_OnResourceAdded();
		}
	}
}

void APlant::DoDamage(int damageAmount)
{
	if (_isBroken)
	{
		return;
	}

	_currentPlantHealth -= damageAmount;
	if (_currentPlantHealth <= 0)
	{
		OnPlayerDestroyPlant();
	}
}

void APlant::OnPlayerDestroyPlant()
{
	_isBroken = true;
	DestroyPlant();
}

void APlant::DestroyPlant()
{
	if (OnPlantBreak.IsBound())
	{
		OnPlantBreak.Broadcast();
	}

	Destroy();
}

void APlant::OnLightAndWaterFilled()
{
	if (_breakCropOnFull && IsLightAndWaterFull())
	{
		OnPlayerDestroyPlant();
	}
}

void APlant::AffectGrowth()
{
	float scaleAmount = FMath::Lerp(_cropData.StartingScaleSize, _cropData.FinalScaleSize, GetCompletionPercentage());

	UStaticMeshComponent* mesh = FindComponentByClass<UStaticMeshComponent>();
	if (ensure(IsValid(mesh)))
	{
		mesh->SetWorldScale3D(FVector::One() * scaleAmount);
	}
}

void APlant::OnDayEnd()
{
	if (_destroyAtEndOfDay)
	{
		DestroyPlant();
	}
}

int APlant::GetCurrentWaterLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTags::Water) : 0;
}

int APlant::GetCurrentLightLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTags::Light) : 0;
}

float APlant::GetCompletionPercentage() const
{
	int water = GetCurrentWaterLevel();
	int light = GetCurrentLightLevel();
	float waterGrowthRatio = water / (float)_cropData.WaterNeeded;
	float lightGrowthRatio = light / (float)_cropData.LightNeeded;
	return (waterGrowthRatio + lightGrowthRatio) / 2.0f;
}

bool APlant::IsLightAndWaterFull() const
{
	return GetCurrentLightLevel() >= _cropData.LightNeeded && GetCurrentWaterLevel() >= _cropData.WaterNeeded;
}
