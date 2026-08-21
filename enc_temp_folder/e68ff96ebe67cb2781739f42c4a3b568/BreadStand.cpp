// Fill out your copyright notice in the Description page of Project Settings.

#include "BreadStand.h"

// Brock
#include "Characters/Customer.h"
#include "Managers/BreadRequirementManager.h"
#include "Managers/CustomerQueue.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Resources/ResourceTypeTags.h"

ABreadStand::ABreadStand() : Super()
{
	_customerQueue = CreateDefaultSubobject<UCustomerQueue>("CustomerQueue");
}

FBreadPropData& ABreadStand::GetBreadPropData(const FGameplayTag& breadType)
{
	return ensure(_breadPropSpawnDatas.Contains(breadType)) ? _breadPropSpawnDatas[breadType] : _emptyData;
}

void ABreadStand::SetBreadPropDataSpawnLocation(const FGameplayTag& breadType, USceneComponent* sceneComponent)
{
	GetBreadPropData(breadType).BreadSpawnLocationComponent = sceneComponent;
}

void ABreadStand::BeginPlay()
{
	Super::BeginPlay();

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayBegin.AddUObject(this, &ABreadStand::OnDayBegin);
		dayNightCycle->OnDayEnd.AddUObject(this, &ABreadStand::OnDayEnd);
	}
}

void ABreadStand::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayBegin.RemoveAll(this);
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

const FVector ABreadStand::GetNextCustomerQueuePosition()
{
	return ensure(IsValid(_customerQueue)) ? _customerQueue->GetEndQueuePosition() : FVector::ZeroVector;
}

const FModifiedResourceValue ABreadStand::GetPriceForResource(const FGameplayTag& resourceType) const
{
	for (const FModifiedResourceValue& priceData : _priceDataArray)
	{
		if (priceData.ResourceTag == resourceType)
		{
			return priceData;
		}
	}

	return FModifiedResourceValue();
}

void ABreadStand::OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount)
{
	if (_isCurrentlySellingBreadToCustomer)
	{
		return;
	}

	if (_breadPropSpawnDatas.Contains(resourceType))
	{
		FBreadPropData& breadPropData = _breadPropSpawnDatas[resourceType];
		int breadCountDifference = amount - breadPropData.BreadPropsInScene.Num();
		if (breadCountDifference > 0 && ensure(IsValid(breadPropData.BreadSpawnLocationComponent)))
		{
			FVector startLoc = breadPropData.BreadSpawnLocationComponent->GetComponentLocation() + (FVector::UpVector * breadPropData.BreadPropHeight * breadPropData.BreadPropsInScene.Num());
			for (int i = 0; i < breadCountDifference; i++)
			{
				AActor* breadProp = GetWorld()->SpawnActor<AActor>(breadPropData.BreadPropClass, startLoc + (FVector::UpVector * breadPropData.BreadPropHeight * i), FRotator::ZeroRotator);
				if (ensure(IsValid(breadProp)))
				{
					breadPropData.BreadPropsInScene.Add(breadProp);
				}
			}
		}
		else if (breadCountDifference < 0)
		{
			int startingCount = breadPropData.BreadPropsInScene.Num() - 1;
			for (int i = startingCount; i > startingCount + breadCountDifference; i--)
			{
				TArray<AActor*>& breadProps = breadPropData.BreadPropsInScene;
				AActor* breadProp = breadProps[i];
				if (ensure(IsValid(breadProp)))
				{
					breadProps.RemoveAt(i);
					breadProp->Destroy();
				}
			}
		}
	}

	if (ensure(IsValid(_customerQueue)))
	{
		ACustomer* customer = _customerQueue->GetFrontOfQueue();
		if (IsValid(customer))
		{
			customer->AttemptBuyBreadAtFrontOfQueue();
		}
	}
}

void ABreadStand::OnDayBegin()
{
}

void ABreadStand::OnDayEnd()
{
	UBreadRequirementManager* breadRequirementManager = UFarmFPSUtilities::GetBreadRequirementManager(this);
	if (ensure(IsValid(breadRequirementManager)) && breadRequirementManager->GetHasSoldBreadRequiredForDay())
	{
		GetOutputInventory()->MultiplyResource(ResourceTypeTags::Money, breadRequirementManager->GetBonusMultiplierForBreadSold());
	}
}
