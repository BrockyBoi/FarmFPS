// Fill out your copyright notice in the Description page of Project Settings.

#include "AutomaticResourceTransferPoint.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Interactables/InputOutputStationActor.h"
#include "Resources/ResourceInventory.h"
#include "Resources/ResourcePickupActor.h"

// UE
#include "Components/PrimitiveComponent.h"

UAutomaticResourceTransferPoint::UAutomaticResourceTransferPoint()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAutomaticResourceTransferPoint::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_playerCollider)))
	{
		_playerCollider->OnComponentBeginOverlap.AddDynamic(this, &UAutomaticResourceTransferPoint::OnComponentOverlap);
	}
}

void UAutomaticResourceTransferPoint::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_playerCollider))
	{
		_playerCollider->OnComponentBeginOverlap.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UAutomaticResourceTransferPoint::SetGivesResources(bool givesResources)
{
	_givesResources = givesResources;
}

void UAutomaticResourceTransferPoint::SetInventory(UResourceInventory* inventory)
{
	_inventory = inventory;
}

void UAutomaticResourceTransferPoint::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	UDayNightCycleManager* dayNightCycleManager = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycleManager)))
	{
		if ((dayNightCycleManager->IsDay() && !_canAccessDuringDay) || (dayNightCycleManager->IsNight() && !_canAccessDuringNight))
		{
			return;
		}
	}

	if (_needsToOverlapPlayer)
	{
		UResourceInventory* inventory = OtherActor->FindComponentByClass<UResourceInventory>();
		if (IsValid(inventory) && ensure(_inventory.IsValid()))
		{
			if (_givesResources)
			{
				inventory->AddAllResourcesInInventory(_inventory.Get());
			}
			else
			{
				for (const FGameplayTag& resourceType : _resourcesAllowed)
				{
					int resourceCount = inventory->GetResourceCount(resourceType);
					if (resourceCount > 0)
					{
						_inventory->AddResource(resourceType, resourceCount);
						inventory->RemoveResource(resourceType, resourceCount);
					}
				}
			}
		}
	}
	else
	{
		AResourcePickupActor* resourcePickup = Cast<AResourcePickupActor>(OtherActor);
		if (IsValid(resourcePickup) && ensure(_inventory.IsValid()) && ensure(IsValid(_parentStation)) && _resourcesAllowed.HasTag(resourcePickup->GetResourceType()))
		{
			if (!_givesResources)
			{
				resourcePickup->AttemptMoveToActor(GetOwner(), _inventory.Get(), _parentStation->GetResourceEndPointLocation());
			}
		}
	}
}

