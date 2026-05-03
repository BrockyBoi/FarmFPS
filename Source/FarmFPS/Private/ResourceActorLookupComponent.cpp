// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceActorLookupComponent.h"

#include "ResourceActorData.h"

UResourceActorLookupComponent::UResourceActorLookupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceActorLookupComponent::BeginPlay()
{
	Super::BeginPlay();
}

TSubclassOf<AResourcePickupActor> UResourceActorLookupComponent::GetResourceActorForType(const FGameplayTag& resourceType) const
{
	if (!_resourceTypeToActorMap.Contains(resourceType))
	{
		if (ensure(IsValid(_dataTable)))
		{
			// Iterate through the internal row map directly
			for (auto It = _dataTable->GetRowMap().CreateConstIterator(); It; ++It)
			{
				FName RowName = It.Key();
				FResourceActorData* row = reinterpret_cast<FResourceActorData*>(It.Value());

				if (ensure(row))
				{
					_resourceTypeToActorMap.Add(row->ResourceType, row->ResourceActorClass);
				}
			}
		}
	}

	if (ensure(_resourceTypeToActorMap.Contains(resourceType)))
	{
		return _resourceTypeToActorMap[resourceType];
	}

	return TSubclassOf<AResourcePickupActor>();
}


