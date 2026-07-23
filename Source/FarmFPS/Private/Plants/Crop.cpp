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
#include "FarmFPSCharacter.h"
#include "Kismet/GameplayStatics.h"

ACrop::ACrop() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	_destroyAtEndOfDay = true;
}

UCapsuleComponent* ACrop::GetCapsuleComponent() const
{
	return FindComponentByClass<UCapsuleComponent>();
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
	_resourcesInventory->ListenToDayCycleEvents(false);
	_isInPerfectTiming = _hasPerfectTimingPeriodEnded = false;
	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	AFarmFPSCharacter* farmFPSCharacter = Cast<AFarmFPSCharacter>(FarmFPSUtilities::GetPlayerCharacter(this));
	if (IsValid(farmFPSCharacter))
	{
		farmFPSCharacter->OnShowCropHealth.RemoveAll(this);
		Cosmetic_OnShouldShowCropHealth(false);
	}

	Cosmetic_OnAddedToPool();
}

void ACrop::RemoveFromPool()
{
	_resourcesInventory->ListenToDayCycleEvents(true);
	_isBroken = _isInPerfectTiming = _hasPerfectTimingPeriodEnded = _isLightAndWaterFull = false;
	_sinAngleInPerfectTiming = 270.f;

	Cosmetic_OnRemovedFromPool();

	UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
	if (ensure(IsValid(objectiveManager)))
	{
		objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::PlantCrop, _cropData.ResourceType);
	}

	InitializeInventory();

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		if (dayNightCycle->IsDay())
		{
			_resourcesInventory->SetCanAddResources(true);
		}

		dayNightCycle->OnDayEnd.AddUObject(this, &ACrop::OnDayEnd);
	}

	AFarmFPSCharacter* farmFPSCharacter = Cast<AFarmFPSCharacter>(FarmFPSUtilities::GetPlayerCharacter(this));
	if (ensure(IsValid(farmFPSCharacter)))
	{
		farmFPSCharacter->OnShowCropHealth.AddUObject(this, &ACrop::Cosmetic_OnShouldShowCropHealth);
		Cosmetic_OnShouldShowCropHealth(farmFPSCharacter->IsShowingCropHealth());
	}

	AffectGrowth();
}

void ACrop::OnPlayerDestroyPlant()
{
	SpawnResourceActors();
	Super::OnPlayerDestroyPlant();
}

void ACrop::SpawnResourceActors()
{
	if (!ensure(IsValid(GetWorld())))
	{
		return;
	}

	int countToDrop = _cropData.NumberOfPickupsToDrop.GetModifiedValue(this);
	int seedsToDrop = 0;
	if (_isInPerfectTiming)
	{
		countToDrop = FMath::RoundToInt(countToDrop * _perfectTimingYieldBonus.GetModifiedValue(this));
	}
	else if (!IsLightAndWaterFull())
	{
		countToDrop = FMath::RoundToInt(countToDrop * GetCompletionPercentage());
	}

	float loveAmount = GetCurrentLoveLevel();
	if (IsFullyLoved())
	{
		seedsToDrop = _cropData.NumberOfLoveSeedsToDrop.GetModifiedValue(this);
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

		for (int i = 0; i < seedsToDrop; i++)
		{
			AActor* seed = actorPool->GetActorFromPool(_cropData.ResourceType, GetActorLocation() + FVector::UpVector * (_yieldPickupSpawnHeight * 2), EPooledActorType::Projectile);
			if (IsValid(seed))
			{
				UPrimitiveComponent* seedCollider = seed->FindComponentByClass<UPrimitiveComponent>();
				if (IsValid(seedCollider))
				{
					seedCollider->AddImpulse(FVector(FMath::RandRange(-25, 25), FMath::RandRange(-25, 25), FMath::RandRange(100, 300)), NAME_None, true);
				}
			}
		}
	}

	UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
	if (ensure(IsValid(objectiveManager)))
	{
		objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::FinishCrop, _cropData.ResourceType);
	}

	if (IsValid(_onBreakCropSound))
	{
		UGameplayStatics::SpawnSoundAtLocation(this, _onBreakCropSound, GetActorLocation());
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
	if (OnPlantBreak.IsBound())
	{
		OnPlantBreak.Broadcast();
	}

	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(_cropData.ResourceType, this, EPooledActorType::Crop);
	}
}

