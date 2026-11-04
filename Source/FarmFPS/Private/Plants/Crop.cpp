// Fill out your copyright notice in the Description page of Project Settings.

#include "Crop.h"

// Brock
#include "FarmFPSCharacter.h"
#include "Managers/ActorPool.h"
#include "Managers/AudioManager.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/PerkManager.h"
#include "Managers/ObjectiveManager.h"
#include "Managers/ObjectiveTypeTag.h"
#include "Misc/SoftObjectHelper.h"
#include "Resources/ResourceInventory.h"
#include "Resources/ResourcePickupActor.h"
#include "Resources/ResourceTypeTag.h"

// UE
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

FOnCropPlanted ACrop::OnCropPlanted;

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

const UCropDataAsset* ACrop::GetCropDataAsset() const
{
	return _cropDataAsset;
}

void ACrop::BeginPlay()
{
	Super::BeginPlay();

	_randomPerfectTimingModifierOffset = FMath::RandRange(-_changeInPerfectTimingSizePerFrame, _changeInPerfectTimingSizePerFrame) * .5f;
}

void ACrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShowCropCompletionVisuals();
}

void ACrop::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACrop::SetMeshForCurrentProgress()
{
	if (!ensure(IsValid(_cropDataAsset)))
	{
		return;
	}

	const TArray<TSoftObjectPtr<UStaticMesh>>& meshes = _cropDataAsset->MeshesOverProgress;
	if (meshes.Num() >= 1)
	{
		int staticMeshIndex = FMath::RoundToInt(meshes.Num() * _previousScalePercentage);
		if (meshes.IsValidIndex(staticMeshIndex))
		{
			SoftObjectHelper::AsyncLoadSoftObject(meshes[staticMeshIndex], FOnAsyncLoad::CreateUObject(this, &ACrop::SetCropMesh));
		}
	}
}

void ACrop::SetCropMesh(UObject* meshObject)
{
	if (UStaticMesh* mesh = Cast<UStaticMesh>(meshObject); ensure(IsValid(mesh)) && ensure(IsValid(_staticMesh)))
	{
		_staticMesh->SetStaticMesh(mesh);
	}
}

void ACrop::SetCropMaterial(UObject* materialInstance)
{
	if (UMaterialInstance* material = Cast<UMaterialInstance>(materialInstance); ensure(IsValid(material)) && ensure(IsValid(_staticMesh)))
	{
		_staticMesh->SetMaterial(0, material);
	}
}

void ACrop::SpawnNiagaraComponent(UObject* niagaraObject, const FGameplayTag& resourceTag)
{
	if (UNiagaraSystem* niagara = Cast<UNiagaraSystem>(niagaraObject); ensure(IsValid(niagara)))
	{
		const FVector scale = FVector::OneVector;
		constexpr bool autoDestroy = false;
		if (UNiagaraComponent* niagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, niagara, GetActorLocation(), GetActorRotation(), scale, autoDestroy, true); ensure(IsValid(niagaraComponent)))
		{
			if (resourceTag == ResourceTypeTag::Water)
			{
				_waterVFX = niagaraComponent;
			}
			else if (resourceTag == ResourceTypeTag::Light)
			{
				_sunVFX = niagaraComponent;
			}
			else if (resourceTag == ResourceTypeTag::Love)
			{
				_loveVFX = niagaraComponent;
			}
		}
	}
}

void ACrop::EnableStartingNiagaraSystems()
{
	if (!ensure(IsValid(_cropDataAsset)))
	{
		return;
	}

	if (!IsValid(_waterVFX))
	{
		SoftObjectHelper::AsyncLoadSoftObject(_cropDataAsset->WaterVFX, FOnAsyncLoad::CreateLambda([&](UObject* niagaraSystem)
			{
				SpawnNiagaraComponent(niagaraSystem, ResourceTypeTag::Water);
			}));
	}
	else
	{
		_waterVFX->SetWorldLocation(GetActorLocation());
		_waterVFX->ActivateSystem();
	}

	if (!IsValid(_sunVFX))
	{
		SoftObjectHelper::AsyncLoadSoftObject(_cropDataAsset->SunVFX, FOnAsyncLoad::CreateLambda([&](UObject* niagaraSystem)
			{
				SpawnNiagaraComponent(niagaraSystem, ResourceTypeTag::Light);
			}));
	}
	else
	{
		_sunVFX->SetWorldLocation(GetActorLocation());
		_sunVFX->ActivateSystem();
	}
}

void ACrop::OnLightAndWaterFilled()
{
	Super::OnLightAndWaterFilled();

	_isInPerfectTiming = true;
	GetWorld()->GetTimerManager().SetTimer(_perfectTimingTimerHandle, this, &ACrop::OnPerfectTimingEnd, _perfectTimingDuration.GetModifiedValue(this), false);
	CheckShouldTick();

	if (ensure(IsValid(_cropDataAsset)))
	{
		SoftObjectHelper::AsyncLoadSoftObject(_cropDataAsset->FinalMaterial, FOnAsyncLoad::CreateUObject(this, &ACrop::SetCropMaterial));
	}
}

void ACrop::OnPerfectTimingEnd()
{
	_isInPerfectTiming = false;
}

void ACrop::AddActorToPool()
{
	_adjustedGrowthPercentage = 0;
	_resourcesInventory->ListenToDayCycleEvents(false);
	_isInPerfectTiming = _hasPerfectTimingPeriodEnded = false;
	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayStateChange.RemoveAll(this);
	}

	AFarmFPSCharacter* farmFPSCharacter = Cast<AFarmFPSCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
	if (IsValid(farmFPSCharacter))
	{
		farmFPSCharacter->OnShowCropHealth.RemoveAll(this);
		Cosmetic_OnShouldShowCropHealth(false);
	}

	if (IsValid(_cropDataAsset))
	{
		SoftObjectHelper::AsyncLoadSoftObject(_cropDataAsset->DestructionParticleSystem, FOnAsyncLoad::CreateLambda([this](UObject* particleSystem)
			{
				if (!IsValid(_onBreakVFX))
				{
					constexpr bool autoDestroy = false;
					constexpr bool autoActivate = false;
					_onBreakVFX = SoftObjectHelper::SpawnParticleSystemComponent(this, particleSystem, GetActorLocation(), GetActorRotation(), _staticMesh->GetRelativeScale3D(), autoDestroy, autoActivate);
				}

				if (ensure(IsValid(_onBreakVFX)))
				{
					_onBreakVFX->SetWorldLocation(GetActorLocation());
					_onBreakVFX->DeactivateImmediate();
					_onBreakVFX->Activate();
				}
			}));

		if (IsValid(_dynamicMaterialInstance))
		{
			_dynamicMaterialInstance->SetVectorParameterValue(_deadFoliageParameterName, _cropDataAsset->StartingDeadFoliageColor);
		}
	}

	if (IsValid(_sunVFX))
	{
		_sunVFX->Deactivate();
	}

	if (IsValid(_waterVFX))
	{
		_waterVFX->Deactivate();
	}

	if (IsValid(_loveVFX))
	{
		_loveVFX->Deactivate();
	}

	ListenToWeatherManager(false);

	Cosmetic_OnAddedToPool();

	CheckShouldTick();
}

void ACrop::RemoveFromPool()
{
	if (ensure(IsValid(_cropDataAsset)))
	{
		SoftObjectHelper::AsyncLoadSoftObject(_cropDataAsset->StandardMaterial, FOnAsyncLoad::CreateLambda([&](UObject* materialInstance)
			{
				SetCropMaterial(materialInstance);
				if (!IsValid(_dynamicMaterialInstance) && ensure(IsValid(_staticMesh)))
				{
					_dynamicMaterialInstance = _staticMesh->CreateDynamicMaterialInstance(0);
				}

				if (ensure(IsValid(_dynamicMaterialInstance)))
				{
					_staticMesh->SetMaterial(0, _dynamicMaterialInstance);
					_dynamicMaterialInstance->SetVectorParameterValue(_deadFoliageParameterName, _cropDataAsset->StartingDeadFoliageColor);
				}
			}));

		EnableStartingNiagaraSystems();
	}

	_resourcesInventory->ListenToDayCycleEvents(true);
	_isBroken = _isInPerfectTiming = _hasPerfectTimingPeriodEnded = _isLightAndWaterFull = false;
	_sinAngleInPerfectTiming = 270.f;

	Cosmetic_OnRemovedFromPool();

	UObjectiveManager* objectiveManager = UFarmFPSUtilities::GetObjectiveManager(this);
	if (ensure(IsValid(objectiveManager)) && ensure(IsValid(_cropDataAsset)))
	{
		objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTag::PlantCrop, _cropDataAsset->ResourceType);
	}

	InitializeInventory();

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		if (dayNightCycle->IsDay())
		{
			_resourcesInventory->SetCanAddResources(true);
		}

		dayNightCycle->OnDayStateChange.RemoveAll(this);
		dayNightCycle->OnDayStateChange.AddUObject(this, &ACrop::OnDayStateChange);
	}

	AFarmFPSCharacter* farmFPSCharacter = Cast<AFarmFPSCharacter>(UFarmFPSUtilities::GetPlayerCharacter(this));
	if (ensure(IsValid(farmFPSCharacter)))
	{
		farmFPSCharacter->OnShowCropHealth.AddUObject(this, &ACrop::Cosmetic_OnShouldShowCropHealth);
		Cosmetic_OnShouldShowCropHealth(farmFPSCharacter->IsShowingCropHealth());
	}

	ListenToWeatherManager(true);

	SetNewGrowthPercentage(_plantGrowthData.StartingScaleSize, true);

	SetMeshForCurrentProgress();

	CheckShouldTick();
}

void ACrop::SetWasPlantedSuccessfully(bool wasSuccessful)
{
	if (wasSuccessful)
	{
		OnCropPlanted.Broadcast(this);
	}
	else
	{

	}
}

void ACrop::AddResource(const FGameplayTag& resourceType, float amount)
{
	Super::AddResource(resourceType, amount);

	SetMeshForCurrentProgress();

	if (ensure(IsValid(_cropDataAsset)))
	{
		if (ensure(IsValid(_dynamicMaterialInstance)) && resourceType == ResourceTypeTag::Water)
		{
			FLinearColor lerpedColor = FLinearColor::LerpUsingHSV(_cropDataAsset->StartingDeadFoliageColor, FLinearColor::White, GetWaterPercentage());
			_dynamicMaterialInstance->SetVectorParameterValue(_deadFoliageParameterName, lerpedColor);
		}
	}

	if (IsFullyLoved())
	{
		if (!IsValid(_loveVFX))
		{
			SoftObjectHelper::AsyncLoadSoftObject(_cropDataAsset->LoveVFX, FOnAsyncLoad::CreateLambda([&](UObject* niagaraSystem)
				{
					SpawnNiagaraComponent(niagaraSystem, ResourceTypeTag::Love);
				}));
		}
		else
		{
			_loveVFX->SetWorldLocation(GetActorLocation());
			_loveVFX->ActivateSystem();
		}
	}

	if (IsWaterFull() && ensure(IsValid(_waterVFX)))
	{
		_waterVFX->Deactivate();
	}

	if (IsLightFull() && ensure(IsValid(_sunVFX)))
	{
		_sunVFX->Deactivate();
	}
}

void ACrop::MeleeObject(int damage)
{
	DoDamage(damage);
}

void ACrop::OnPlayerDestroyPlant()
{
	SpawnResourceActors();
	Super::OnPlayerDestroyPlant();
}

void ACrop::SpawnResourceActors()
{
	if (!ensure(IsValid(GetWorld())) || !ensure(IsValid(_cropDataAsset)))
	{
		return;
	}

	int countToDrop = _cropDataAsset->NumberOfPickupsToDrop.GetValue().GetModifiedValue(this);
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
		seedsToDrop = _cropDataAsset->NumberOfLoveSeedsToDrop.GetValue().GetModifiedValue(this);
	}

	UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		for (int i = 0; i < countToDrop; i++)
		{
			AActor* pickup = actorPool->GetActorFromPool(_cropDataAsset->ResourceType, GetActorLocation() + FVector::UpVector * _yieldPickupSpawnHeight, EPooledActorType::ResourcePickup);

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
			AActor* seed = actorPool->GetActorFromPool(_cropDataAsset->ResourceType, GetActorLocation() + FVector::UpVector * (_yieldPickupSpawnHeight * 2), EPooledActorType::Projectile);
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

	UObjectiveManager* objectiveManager = UFarmFPSUtilities::GetObjectiveManager(this);
	if (ensure(IsValid(objectiveManager)))
	{
		objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTag::FinishCrop, _cropDataAsset->ResourceType);
	}

	if (ensure(IsValid(_cropDataAsset)))
	{
		SoftObjectHelper::PlaySpawnedSoundAtLocation(this, _cropDataAsset->OnBreakCropSound, GetActorLocation());
	}
}

void ACrop::ShowCropCompletionVisuals()
{
	if (!IsLightAndWaterFull())
	{
		return;
	}

	UStaticMeshComponent* cropMesh = FindComponentByClass<UStaticMeshComponent>();
	if (ensure(IsValid(cropMesh)))
	{
		float scale = FMath::Lerp(_plantGrowthData.FinalScaleSize, _plantGrowthData.FinalScaleSize * _maxSizeModifierForPerfectTiming, FMath::Sin(FMath::DegreesToRadians(_sinAngleInPerfectTiming)) + 1);
		cropMesh->SetWorldScale3D(FVector::One() * scale);
		_sinAngleInPerfectTiming += (_changeInPerfectTimingSizePerFrame + _randomPerfectTimingModifierOffset);

		//if (_hasPerfectTimingPeriodEnded)
		//{
		//	FVector currentScale = cropMesh->GetRelativeScale3D();
		//	if (currentScale.X > _cropData.FinalScaleSize)
		//	{
		//		cropMesh->SetWorldScale3D(currentScale * .995f);
		//	}
		//	else if (currentScale.X < _cropData.FinalScaleSize)
		//	{
		//		cropMesh->SetWorldScale3D(currentScale * 1.015f);
		//	}

		//	if ((currentScale - _cropData.FinalScaleSize).IsNearlyZero(.01))
		//	{
		//		cropMesh->SetWorldScale3D(FVector::One() * _cropData.FinalScaleSize);
		//		CheckShouldTick();
		//	}
		//}
	}
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

	UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)) && ensure(IsValid(_cropDataAsset)))
	{
		actorPool->AddActorToPool(_cropDataAsset->ResourceType, this, EPooledActorType::Crop);
	}
}

bool ACrop::ShouldTick() const
{
	return IsLightAndWaterFull() || Super::ShouldTick();
}