// Fill out your copyright notice in the Description page of Project Settings.

#include "Plant.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/Weather/WeatherManager.h"
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

	_weatherManager = UFarmFPSUtilities::GetWeatherManager(this);

	InitializeInventory();
	ListenToWeatherManager(true);
	CheckShouldTick();

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &APlant::OnDayEnd);
	}
}

void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensure(_weatherManager.IsValid()) && _weatherManager->IsStorming())
	{
		AddResource(_weatherManager->GetCurrentStormTags(), _weatherManager->GetCurrentStormIntensity() * DeltaTime);
	}
}

void APlant::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ListenToWeatherManager(false);

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
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
			if (!_resourcesInventory->CanAddResource(resourceType))
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

			if (_resourcesInventory->IsResourceFull(resourceType))
			{
				Cosmetic_OnResourceFull(resourceType);
				OnPlantResourceFull.Broadcast(resourceType);
			}

			Cosmetic_OnResourceAdded();
		}
	}
}

void APlant::AddResource(const FGameplayTagContainer& resourceTypes, float amount)
{
	for (const FGameplayTag& resourceType : resourceTypes)
	{
		AddResource(resourceType, amount);
	}
}

void APlant::DoDamage(int damageAmount)
{
	if (_isBroken || _cropData.CropHealth.GetModifiedValue(this) <= 0)
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

void APlant::InitializeInventory()
{
	_resourcesInventory = FindComponentByClass<UResourceInventory>();
	if (ensure(IsValid(_resourcesInventory)))
	{
		_resourcesInventory->SetResourceAmount(ResourceTypeTags::Water, 0);
		_resourcesInventory->SetResourceAmount(ResourceTypeTags::Light, 0);
		_resourcesInventory->SetResourceAmount(ResourceTypeTags::Love, 0);

		_resourcesInventory->SetResourceCap(ResourceTypeTags::Water, _cropData.WaterNeeded.GetModifiedValue(this));
		_resourcesInventory->SetResourceCap(ResourceTypeTags::Light, _cropData.LightNeeded.GetModifiedValue(this));
		_resourcesInventory->SetResourceCap(ResourceTypeTags::Love, _cropData.LoveNeeded.GetModifiedValue(this));
	}
}

void APlant::ListenToWeatherManager(bool listen)
{
	if (_weatherManager.IsValid())
	{
		_weatherManager->OnStormStarted.RemoveAll(this);
		_weatherManager->OnStormEnded.RemoveAll(this);

		if (listen)
		{
			_weatherManager->OnStormStarted.AddUObject(this, &APlant::OnStormBegin);
			_weatherManager->OnStormEnded.AddUObject(this, &APlant::OnStormEnd);
		}
	}
}

bool APlant::ShouldTick() const
{
	return _weatherManager.IsValid() && _weatherManager->IsStorming();
}

void APlant::CheckShouldTick()
{
	SetActorTickEnabled(ShouldTick());
}

void APlant::OnStormBegin()
{
	CheckShouldTick();
}

void APlant::OnStormEnd()
{
	CheckShouldTick();
}

void APlant::DestroyPlant()
{
	if (OnPlantBreak.IsBound())
	{
		OnPlantBreak.Broadcast();
	}

	ListenToWeatherManager(false);

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

	if (ensure(IsValid(_resourcesInventory)) && _resourcesInventory->ClearsResourcesAtEndOfDay())
	{
		_resourcesInventory->ClearAllExceptMoney();
		AffectGrowth();
		Cosmetic_OnResourceAdded();
	}
}

float APlant::GetCurrentWaterLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTags::Water) : 0;
}

float APlant::GetCurrentLightLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTags::Light) : 0;
}

float APlant::GetCurrentLoveLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTags::Love) : 0;
}

float APlant::GetCompletionPercentage() const
{
	int water = GetCurrentWaterLevel();
	int light = GetCurrentLightLevel();
	float waterGrowthRatio = water / (float)_cropData.WaterNeeded.GetModifiedValue(this);
	float lightGrowthRatio = light / (float)_cropData.LightNeeded.GetModifiedValue(this);
	return (waterGrowthRatio + lightGrowthRatio) / 2.0f;
}

bool APlant::IsLightAndWaterFull() const
{
	return FMath::IsNearlyEqual(GetCurrentLightLevel(), _cropData.LightNeeded.GetModifiedValue(this)) && FMath::IsNearlyEqual(GetCurrentWaterLevel(), _cropData.WaterNeeded.GetModifiedValue(this));
}

bool APlant::IsFullyLoved() const
{
	return _cropData.LoveNeeded.GetBaseValue() > 0 && FMath::IsNearlyEqual(GetCurrentLoveLevel(), _cropData.LoveNeeded.GetModifiedValue(this));
}
