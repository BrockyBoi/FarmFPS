// Fill out your copyright notice in the Description page of Project Settings.

#include "PurchaseLocation.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/PerkManager.h"
#include "Managers/PerkModifierTypeTag.h"
#include "Resources/ResourceInventory.h"
#include "Resources/ResourceTypeTag.h"

UPurchaseLocation::UPurchaseLocation()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPurchaseLocation::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_overlappingComponent)))
	{
		_overlappingComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentOverlap);
	}

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayBegin.AddUObject(this, &ThisClass::OnDayBegin);
		dayNightCycle->OnDayEnd.AddUObject(this, &ThisClass::OnDayEnd);
	}
}

void UPurchaseLocation::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	if (IsValid(_overlappingComponent))
	{
		_overlappingComponent->OnComponentBeginOverlap.RemoveAll(this);
	}

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayBegin.RemoveAll(this);
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	Super::EndPlay(endPlayReason);
}

void UPurchaseLocation::OnDayBegin()
{
	HidePurchaseLocation(true);
}

void UPurchaseLocation::OnDayEnd()
{
	if (_canPurchase)
	{
		HidePurchaseLocation(false);
	}
}

void UPurchaseLocation::HidePurchaseLocation(bool shouldHide)
{
	GetOwner()->SetActorHiddenInGame(shouldHide);
	GetOwner()->SetActorEnableCollision(!shouldHide);
}

void UPurchaseLocation::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (!ensure(IsValid(dayNightCycle)) || dayNightCycle->IsDay())
	{
		return;
	}

	if (IsValid(OtherActor) && IsValid(OtherComp))
	{
		UPerkManager* perkManager = OtherActor->FindComponentByClass<UPerkManager>();
		UResourceInventory* inventory = OtherActor->FindComponentByClass<UResourceInventory>();
		if (IsValid(inventory) && IsValid(perkManager))
		{
			if (AttemptPurchase(perkManager, inventory))
			{
				UE_LOG(LogTemp, Log, TEXT("Purchase successful!"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Purchase failed!"));
			}
		}
	}
}

bool UPurchaseLocation::AttemptPurchase(UPerkManager* perkManager, UResourceInventory* inventory)
{
	if (ensure(IsValid(perkManager)) && ensure(IsValid(inventory)) && _canPurchase)
	{
		float moneyNeeded = perkManager->ModifyValueByPerks(PerkModifierTypeTag::MoneyNeededForUpgrades, _purchaseCost);
		if (inventory->HasResourceAmount(ResourceTypeTag::Money, moneyNeeded))
		{
			inventory->RemoveResource(ResourceTypeTag::Money, moneyNeeded);
			OnPurchaseSuccess(perkManager, inventory);
			return true;
		}
	}
	return false;
}

void UPurchaseLocation::OnPurchaseSuccess(UPerkManager* perkManager, UResourceInventory* inventory)
{
	_purchaseCost *= _purchaseCostMultiplier;
	_currentPurchaseCount++;

	Cosmetic_OnPurchaseSuccess.Broadcast();

	if (_currentPurchaseCount >= _maxPurchaseCount)
	{
		SetCanPurchase(false);
		HidePurchaseLocation(true);
	}
}
