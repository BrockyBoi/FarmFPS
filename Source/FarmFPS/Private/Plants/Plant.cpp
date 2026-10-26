// Fill out your copyright notice in the Description page of Project Settings.

#include "Plant.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/Weather/WeatherManager.h"
#include "Resources/ResourceInventory.h"
#include "Resources/ResourceTypeTag.h"
#include "SaveSystem/SaveGameManager.h"

FOnFullyGrown APlant::OnFullyGrown;

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
		dayNightCycle->OnDayStateChange.AddUObject(this, &APlant::OnDayStateChange);
	}

	USaveGameManager* saveGame = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGame)))
	{
		saveGame->OnLoadGameData.AddUObject(this, &APlant::OnGameLoaded);
	}

	_staticMesh = FindComponentByClass<UStaticMeshComponent>();
	ensure(IsValid(_staticMesh));
}

void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensure(_weatherManager.IsValid()) && _weatherManager->IsStorming())
	{
		AddResource(_weatherManager->GetCurrentStormTags(), _weatherManager->GetCurrentStormIntensity() * DeltaTime);
	}

	if (!FMath::IsNearlyZero(_adjustedGrowthPercentage))
	{
		AffectGrowth();
	}
}

void APlant::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ListenToWeatherManager(false);

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayStateChange.RemoveAll(this);
	}

	USaveGameManager* saveGame = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGame))
	{
		saveGame->OnLoadGameData.RemoveAll(this);
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
		if (resourceType == ResourceTypeTag::Damage)
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

			SetNewGrowthPercentage(GetLightPercentage(), false);

			if (!_isLightAndWaterFull && IsLightAndWaterFull())
			{
				OnLightAndWaterFilled();
				_isLightAndWaterFull = true;
				OnFullyGrown.Broadcast();
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
	if (_isBroken || _cropData.CropHealth.GetModifiedValue(this) <= 0 || _isInvincible)
	{
		return;
	}

	_currentPlantHealth -= damageAmount;
	if (_currentPlantHealth <= 0)
	{
		OnPlayerDestroyPlant();
	}
}

void APlant::SetIsInvincible(bool isInvincible)
{
	_isInvincible = isInvincible;
}

void APlant::OnGameLoaded(UFarmFPSSaveGame* saveGame)
{
	if (ensure(saveGame))
	{
		const FPlantSaveData* plantSaveData = saveGame->GetPlantSaveDatas().FindByPredicate([this](const FPlantSaveData& data)
			{
				return data.PlantName == GetName();
			});

		if (ensure(plantSaveData) && plantSaveData->IsBroken)
		{
			OnPlayerDestroyPlant();
		}
	}
}

FPlantSaveData APlant::GetPlantSaveData() const
{
	FPlantSaveData saveData;
	if (!_destroyAtEndOfDay)
	{
		saveData.PlantName = GetName();
		saveData.IsBroken = _isBroken;
	}

	return saveData;
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
		_resourcesInventory->SetResourceAmount(ResourceTypeTag::Water, 0);
		_resourcesInventory->SetResourceAmount(ResourceTypeTag::Light, 0);
		_resourcesInventory->SetResourceAmount(ResourceTypeTag::Love, 0);

		_resourcesInventory->SetResourceCap(ResourceTypeTag::Water, _cropData.WaterNeeded.GetModifiedValue(this));
		_resourcesInventory->SetResourceCap(ResourceTypeTag::Light, _cropData.LightNeeded.GetModifiedValue(this));
		_resourcesInventory->SetResourceCap(ResourceTypeTag::Love, _cropData.LoveNeeded.GetModifiedValue(this));
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
	return (_weatherManager.IsValid() && _weatherManager->IsStorming()) || !FMath::IsNearlyZero(_adjustedGrowthPercentage);
}

void APlant::CheckShouldTick()
{
	SetActorTickEnabled(ShouldTick());
}

void APlant::SetNewGrowthPercentage(float growthPercentage, bool instant)
{
	UStaticMeshComponent* mesh = FindComponentByClass<UStaticMeshComponent>();
	if (ensure(IsValid(mesh)))
	{
		if (!instant)
		{
			_previousScalePercentage = FMath::GetRangePct(_cropData.StartingScaleSize, _cropData.FinalScaleSize, mesh->GetRelativeTransform().GetScale3D().X);
			_adjustedGrowthPercentage = FMath::Clamp(growthPercentage, 0, 1.f);
			CheckShouldTick();
		}
		else
		{
			mesh->SetRelativeScale3D(FVector::OneVector * growthPercentage);
			_adjustedGrowthPercentage = 0.f;
			_previousScalePercentage = FMath::GetRangePct(_cropData.StartingScaleSize, _cropData.FinalScaleSize, mesh->GetRelativeTransform().GetScale3D().X);
		}
	}

}

void APlant::OnStormBegin()
{
	CheckShouldTick();
}

void APlant::OnStormEnd()
{
	CheckShouldTick();
}

void APlant::OnDayStateChange(EDayState newState)
{
	if (newState == EDayState::MidNight)
	{
		if (_destroyAtEndOfDay)
		{
			DestroyPlant();
		}

		if (ensure(IsValid(_resourcesInventory)) && _resourcesInventory->ClearsResourcesAtEndOfDay())
		{
			_resourcesInventory->SetCanAddResources(false);
			_resourcesInventory->ClearAllExceptMoney();
			SetNewGrowthPercentage(_cropData.StartingScaleSize, true);
			Cosmetic_OnResourceAdded();
		}
	}
}

void APlant::DestroyPlant()
{
	if (OnPlantBreak.IsBound())
	{
		OnPlantBreak.Broadcast();
	}

	ListenToWeatherManager(false);

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
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
	float lerpedScale = 0.f;
	UStaticMeshComponent* mesh = FindComponentByClass<UStaticMeshComponent>();
	if (ensure(IsValid(mesh)))
	{
		if (!FMath::IsNearlyZero(_adjustedGrowthPercentage))
		{
			_previousScalePercentage = FMath::Lerp(_previousScalePercentage, _adjustedGrowthPercentage, _lerpSpeedScale);
			lerpedScale = FMath::Lerp(_cropData.StartingScaleSize, _cropData.FinalScaleSize, _previousScalePercentage);
			if (FMath::IsNearlyEqual(_previousScalePercentage, _adjustedGrowthPercentage, .01f))
			{
				_adjustedGrowthPercentage = 0;
				_previousScalePercentage = 0;
			}
		}

		mesh->SetWorldScale3D(FVector::One() * lerpedScale);
	}

	CheckShouldTick();
}

float APlant::GetCurrentWaterLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTag::Water) : 0;
}

float APlant::GetCurrentLightLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTag::Light) : 0;
}

float APlant::GetCurrentLoveLevel() const
{
	return ensure(IsValid(_resourcesInventory)) ? _resourcesInventory->GetResourceCount(ResourceTypeTag::Love) : 0;
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
