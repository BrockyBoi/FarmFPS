// Fill out your copyright notice in the Description page of Project Settings.

#include "CropComponent.h"

// Brock
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
	PrimaryComponentTick.bCanEverTick = false;
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

	AffectGrowth();
}

void UCropComponent::AddCropResourceValue(const FGameplayTag& resourceType, float amount)
{
	if (ensure(IsValid(_cropResourcesInventory)))
	{
		_cropResourcesInventory->AddResource(resourceType, amount);

		AffectGrowth();
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

bool UCropComponent::IsCropReadyToBreak() const
{
	return GetCurrentLightLevel() >= _cropData.LightNeeded && GetCurrentWaterLevel() >= _cropData.WaterNeeded;
}

void UCropComponent::BreakCrop()
{
	if (_isBroken)
	{
		return;
	}

	if (ensure(IsValid(GetWorld())) && ensure(IsValid(GetOwner())) && ensure(IsValid(_cropYieldPickupClass)))
	{
		UPerkManager* perkManager = FarmFPSUtilities::GetPlayerPerkManager(this);
		if (!ensure(IsValid(perkManager)))
		{
			return;
		}

		_isBroken = true;

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

		UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
		if (ensure(IsValid(objectiveManager)))
		{
			objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::FinishCrop, _cropData.ResourceType);
		}

		if (OnCropBreak.IsBound())
		{
			OnCropBreak.Broadcast();
		}

		GetOwner()->Destroy();
		//FTimerHandle handle;
		//GetWorld()->GetTimerManager().SetTimer(handle, this, &UCropComponent::OnBreakCropTimerEnd, .1f, false);
	}
}

void UCropComponent::AffectGrowth()
{
	if (ensure(IsValid(GetOwner())))
	{
		int water = GetCurrentWaterLevel();
		int light = GetCurrentLightLevel();
		float waterGrowthRatio = water / (float)_cropData.WaterNeeded;
		float lightGrowthRatio = light / (float)_cropData.LightNeeded;
		float scaleAmount = FMath::Lerp(_cropData.StartingScaleSize, _cropData.FinalScaleSize, (waterGrowthRatio + lightGrowthRatio) / 2);

		UStaticMeshComponent* cropMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		if (ensure(IsValid(cropMesh)))
		{
			cropMesh->SetWorldScale3D(FVector::One() * scaleAmount);
		}

		if (_breakCropOnFull && IsCropReadyToBreak())
		{
			BreakCrop();
		}
	}
}

void UCropComponent::OnBreakCropTimerEnd()
{
	GetOwner()->Destroy();
}

