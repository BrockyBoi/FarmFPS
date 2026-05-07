// Fill out your copyright notice in the Description page of Project Settings.

#include "CropComponent.h"

// Brock
#include "ActorPool.h"
#include "DayNightCycleManager.h"
#include "FarmFPSUtilities.h"
#include "PerkManager.h"
#include "PerkModifierTypeTags.h"
#include "ObjectiveManager.h"
#include "ObjectiveTypeTags.h"
#include "ResourceInventory.h"
#include "ResourcePickupActor.h"
#include "ResourceTypeTags.h"

UCropComponent::UCropComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCropComponent::BeginPlay()
{
	Super::BeginPlay();

	UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
	if (ensure(IsValid(objectiveManager)))
	{
		objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::PlantCrop, _cropData.ResourceType);
	}

	if (ensure(IsValid(GetOwner())))
	{
		_cropResourcesInventory = GetOwner()->FindComponentByClass<UResourceInventory>();
		if (ensure(IsValid(_cropResourcesInventory)))
		{
			_cropResourcesInventory->SetResourceCap(ResourceTypeTags::Water, _cropData.WaterNeeded);
			_cropResourcesInventory->SetResourceCap(ResourceTypeTags::Light, _cropData.LightNeeded);
		}
	}

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &UCropComponent::OnDayEnd);
	}

	AffectGrowth();
}

void UCropComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	ShowPerfectTimingVisuals();
}

void UCropComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UCropComponent::OnLightAndWaterFilled()
{
	_isInPerfectTiming = true;
	GetWorld()->GetTimerManager().SetTimer(_perfectTimingTimerHandle, this, &UCropComponent::OnPerfectTimingEnd, _perfectTimingDuration.GetModifiedValue(this), false);
	SetComponentTickEnabled(true);
}

void UCropComponent::OnPerfectTimingEnd()
{
	_isInPerfectTiming = false;
}

void UCropComponent::AddCropResourceValue(const FGameplayTag& resourceType, float amount)
{
	if (ensure(IsValid(_cropResourcesInventory)))
	{
		_cropResourcesInventory->AddResource(resourceType, amount);

		AffectGrowth();

		if (!_hasStartedPerfectTiming && IsLightAndWaterFull())
		{
			OnLightAndWaterFilled();
			_hasStartedPerfectTiming = true;
		}
	}
}

void UCropComponent::DoDamageToCrop(int damageAmount)
{
	_currentCropHealth -= damageAmount;
	if (_currentCropHealth <= 0)
	{
		BreakCrop();
	}
}

int UCropComponent::GetCurrentWaterLevel() const
{
	return ensure(IsValid(_cropResourcesInventory)) ? _cropResourcesInventory->GetResourceCount(ResourceTypeTags::Water) : 0;
}

int UCropComponent::GetCurrentLightLevel() const
{
	return ensure(IsValid(_cropResourcesInventory)) ? _cropResourcesInventory->GetResourceCount(ResourceTypeTags::Light) : 0;
}

float UCropComponent::GetCropCompletionPercentage() const
{
	int water = GetCurrentWaterLevel();
	int light = GetCurrentLightLevel();
	float waterGrowthRatio = water / (float)_cropData.WaterNeeded;
	float lightGrowthRatio = light / (float)_cropData.LightNeeded;
	return (waterGrowthRatio + lightGrowthRatio) / 2.0f;
}

bool UCropComponent::IsLightAndWaterFull() const
{
	return GetCurrentLightLevel() >= _cropData.LightNeeded && GetCurrentWaterLevel() >= _cropData.WaterNeeded;
}

void UCropComponent::BreakCrop()
{
	if (_isBroken)
	{
		return;
	}

	if (ensure(IsValid(GetWorld())) && ensure(IsValid(GetOwner())))
	{
		UPerkManager* perkManager = FarmFPSUtilities::GetPlayerPerkManager(this);
		if (!ensure(IsValid(perkManager)))
		{
			return;
		}

		_isBroken = true;

		int countToDrop = perkManager->ModifyValueByPerks(PerkModifierTypeTags::MoreCropYield, _cropData.NumberOfPickupsToDrop);
		if (_isInPerfectTiming)
		{
			countToDrop = FMath::RoundToInt(countToDrop * _perfectTimingYieldBonus.GetModifiedValue(this));
		}
		else if (!IsLightAndWaterFull())
		{
			countToDrop = FMath::RoundToInt(countToDrop * GetCropCompletionPercentage());
		}

		UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
		if (ensure(IsValid(actorPool)))
		{
			for (int i = 0; i < countToDrop; i++)
			{
				AActor* pickup = actorPool->GetActorFromPool(_cropData.ResourceType, GetOwner()->GetActorLocation() + FVector::UpVector * _yieldPickupSpawnHeight, EPooledActorType::ResourcePickup);

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
		}

		UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
		if (ensure(IsValid(objectiveManager)))
		{
			objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::FinishCrop, _cropData.ResourceType);
		}

		if (OnCropBreak.IsBound())
		{
			OnCropBreak.Broadcast();
		}

		//GetOwner()->Destroy();
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &UCropComponent::OnBreakCropTimerEnd, .1f, false);
	}
}

void UCropComponent::AffectGrowth()
{
	if (ensure(IsValid(GetOwner())))
	{
		float scaleAmount = FMath::Lerp(_cropData.StartingScaleSize, _cropData.FinalScaleSize, GetCropCompletionPercentage());

		UStaticMeshComponent* cropMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		if (ensure(IsValid(cropMesh)))
		{
			cropMesh->SetWorldScale3D(FVector::One() * scaleAmount);
		}

		if (_breakCropOnFull && IsLightAndWaterFull())
		{
			BreakCrop();
		}
	}
}

void UCropComponent::ShowPerfectTimingVisuals()
{
	if (ensure(IsValid(GetOwner())))
	{
		UStaticMeshComponent* cropMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		if (ensure(IsValid(cropMesh)))
		{
			if (_isInPerfectTiming)
			{
				float scale = FMath::Lerp(_cropData.FinalScaleSize, _cropData.FinalScaleSize * _maxSizeModifierForPerfectTiming, FMath::Sin(FMath::DegreesToRadians(_sinAngleInPerfectTiming)) + 1);
				cropMesh->SetWorldScale3D(FVector::One() * scale);
			}
			else
			{
				FVector currentScale = cropMesh->GetRelativeScale3D();
				if (currentScale.X > _cropData.FinalScaleSize)
				{
					cropMesh->SetWorldScale3D(currentScale * .995f);
				}
				else if (currentScale.X < _cropData.FinalScaleSize)
				{
					cropMesh->SetWorldScale3D(currentScale * 1.015f);
				}

				if ((currentScale - _cropData.FinalScaleSize).IsNearlyZero(.01))
				{
					cropMesh->SetWorldScale3D(FVector::One() * _cropData.FinalScaleSize);
					SetComponentTickEnabled(false);
				}
			}
		}

		_sinAngleInPerfectTiming -= .75f;
	}
}

void UCropComponent::OnDayEnd()
{
	DestroyCrop();
}

void UCropComponent::OnBreakCropTimerEnd()
{
	DestroyCrop();
}

void UCropComponent::DestroyCrop()
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(_cropData.ResourceType, GetOwner(), EPooledActorType::Crop);
	}
}

