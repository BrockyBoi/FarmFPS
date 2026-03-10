// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradePurchaceLocation.h"

// Brock
#include "PerkManager.h"
#include "ResourceInventory.h"

UUpgradePurchaceLocation::UUpgradePurchaceLocation()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUpgradePurchaceLocation::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_overlappingComponent)))
	{
		_overlappingComponent->OnComponentBeginOverlap.AddDynamic(this, &UUpgradePurchaceLocation::OnComponentOverlap);
	}
}

void UUpgradePurchaceLocation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_overlappingComponent))
	{
		_overlappingComponent->OnComponentBeginOverlap.RemoveAll(this);
	}
}

void UUpgradePurchaceLocation::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && IsValid(OtherComp))
	{
		UPerkManager* perkManager = OtherActor->FindComponentByClass<UPerkManager>();
		UResourceInventory* inventory = OtherActor->FindComponentByClass<UResourceInventory>();
		if (IsValid(inventory) && IsValid(perkManager))
		{
			if (AttemptUpgradePurchase(perkManager, inventory))
			{
				UE_LOG(LogTemp, Log, TEXT("Upgrade purchased!"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Upgrade purchase failed!"));
			}
		}
	}
}

bool UUpgradePurchaceLocation::AttemptUpgradePurchase(UPerkManager* perkManager, UResourceInventory* inventory)
{
	if (ensure(IsValid(perkManager)) && ensure(IsValid(inventory)))
	{
		float moneyNeeded = perkManager->ModifyValueByPerks(EPerkModifiers::MoneyNeededForUpgrades, _upgradeCost);
		if (inventory->HasResourceAmount(EResourceType::Money, moneyNeeded))
		{
			inventory->RemoveResource(EResourceType::Money, moneyNeeded);
			perkManager->ModifyPerkData(_upgradeType, _perkUpgradeAmount);
			_upgradeCost *= _upgradeCostMultiplier;
			return true;
		}
	}
	return false;
}

