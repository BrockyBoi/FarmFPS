// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomaticResourceTransferPoint.h"

// Brock
#include "ActorPool.h"
#include "FarmFPSUtilities.h"
#include "ResourceInventory.h"
#include "ResourcePickupActor.h"

// UE
#include "Components/PrimitiveComponent.h"

UAutomaticResourceTransferPoint::UAutomaticResourceTransferPoint()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
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
	if (IsValid(OtherActor))
	{
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
			if (IsValid(resourcePickup) && ensure(_inventory.IsValid()) && _resourcesAllowed.HasTag(resourcePickup->GetResourceType()))
			{
				if (!_givesResources)
				{
					_inventory->AddResource(resourcePickup->GetResourceType(), 1);
					UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
					if (ensure(IsValid(actorPool)))
					{
						actorPool->AddActorToPool(resourcePickup->GetResourceType(), resourcePickup, EPooledActorType::ResourcePickup);
					}
				}
			}
		}
	}
}

