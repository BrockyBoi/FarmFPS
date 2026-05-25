// Fill out your copyright notice in the Description page of Project Settings.

#include "Crop.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/PerkManager.h"
#include "Managers/PerkModifierTypeTags.h"
#include "Managers/ObjectiveManager.h"
#include "Managers/ObjectiveTypeTags.h"
#include "Resources/ResourceInventory.h"
#include "Resources/ResourcePickupActor.h"
#include "Resources/ResourceTypeTags.h"

// UE
#include "Kismet/GameplayStatics.h"

ACrop::ACrop()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	_destroyAtEndOfDay = true;
}

void ACrop::BeginPlay()
{
	Super::BeginPlay();
}

void ACrop::Tick(float DeltaTime)
{
	ShowPerfectTimingVisuals();
}

void ACrop::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACrop::OnLightAndWaterFilled()
{
	Super::OnLightAndWaterFilled();

	_isInPerfectTiming = true;
	GetWorld()->GetTimerManager().SetTimer(_perfectTimingTimerHandle, this, &ACrop::OnPerfectTimingEnd, _perfectTimingDuration.GetModifiedValue(this), false);
	SetActorTickEnabled(true);
}

void ACrop::OnPerfectTimingEnd()
{
	_isInPerfectTiming = false;
}

void ACrop::AddActorToPool()
{
	_isInPerfectTiming = _hasPerfectTimingPeriodEnded = false;
	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}
}

void ACrop::RemoveFromPool()
{
	_isBroken = false;
	_isInPerfectTiming = _hasPerfectTimingPeriodEnded = false;
	_sinAngleInPerfectTiming = 270.f;

	Cosmetic_OnRemovedFromPool();

	UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
	if (ensure(IsValid(objectiveManager)))
	{
		objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::PlantCrop, _cropData.ResourceType);
	}

	_cropResourcesInventory = FindComponentByClass<UResourceInventory>();
	if (ensure(IsValid(_cropResourcesInventory)))
	{
		_cropResourcesInventory->SetResourceAmount(ResourceTypeTags::Water, 0);
		_cropResourcesInventory->SetResourceAmount(ResourceTypeTags::Light, 0);

		_cropResourcesInventory->SetResourceCap(ResourceTypeTags::Water, _cropData.WaterNeeded);
		_cropResourcesInventory->SetResourceCap(ResourceTypeTags::Light, _cropData.LightNeeded);
	}

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &ACrop::OnDayEnd);
	}

	AffectGrowth();
}

void ACrop::SpawnResourceActors()
{
	if (ensure(IsValid(GetWorld())))
	{
		UPerkManager* perkManager = FarmFPSUtilities::GetPlayerPerkManager(this);
		if (!ensure(IsValid(perkManager)))
		{
			return;
		}

		int countToDrop = perkManager->ModifyValueByPerks(PerkModifierTypeTags::MoreCropYield, _cropData.NumberOfPickupsToDrop);
		if (_isInPerfectTiming)
		{
			countToDrop = FMath::RoundToInt(countToDrop * _perfectTimingYieldBonus.GetModifiedValue(this));
		}
		else if (!IsLightAndWaterFull())
		{
			countToDrop = FMath::RoundToInt(countToDrop * GetCompletionPercentage());
		}

		UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
		if (ensure(IsValid(actorPool)))
		{
			for (int i = 0; i < countToDrop; i++)
			{
				AActor* pickup = actorPool->GetActorFromPool(_cropData.ResourceType, GetActorLocation() + FVector::UpVector * _yieldPickupSpawnHeight, EPooledActorType::ResourcePickup);

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

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ACrop::OnBreakCropTimerEnd, .1f, false);

		if (ensure(IsValid(_onBreakCropSound)))
		{
			UGameplayStatics::SpawnSoundAtLocation(this, _onBreakCropSound, GetActorLocation());
		}
	}
}

void ACrop::ShowPerfectTimingVisuals()
{
	if (!_isInPerfectTiming && !_hasPerfectTimingPeriodEnded)
	{
		return;
	}

	UStaticMeshComponent* cropMesh = FindComponentByClass<UStaticMeshComponent>();
	if (ensure(IsValid(cropMesh)))
	{
		if (_isInPerfectTiming)
		{
			float scale = FMath::Lerp(_cropData.FinalScaleSize, _cropData.FinalScaleSize * _maxSizeModifierForPerfectTiming, FMath::Sin(FMath::DegreesToRadians(_sinAngleInPerfectTiming)) + 1);
			cropMesh->SetWorldScale3D(FVector::One() * scale);
		}
		else if (_hasPerfectTimingPeriodEnded)
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
				SetActorTickEnabled(false);
			}
		}
	}

	_sinAngleInPerfectTiming -= .75f;
}

void ACrop::OnBreakCropTimerEnd()
{
	DestroyPlant();
}

void ACrop::DestroyPlant()
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(_cropData.ResourceType, this, EPooledActorType::Crop);
	}
}

