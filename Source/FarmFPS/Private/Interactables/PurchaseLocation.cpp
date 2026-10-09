// Fill out your copyright notice in the Description page of Project Settings.

#include "PurchaseLocation.h"

// Brock
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/PerkManager.h"
#include "Managers/PerkModifierTypeTag.h"
#include "Resources/ResourceInventory.h"
#include "Resources/ResourceTypeTag.h"
#include "SaveSystem/FarmFPSSaveGame.h"
#include "SaveSystem/SaveGameManager.h"
#include "SaveSystem/UpgradeLocationSaveData.h"

UPurchaseLocation::FStaticOnPurchaseSuccess UPurchaseLocation::StaticOnPurchaseSuccess;

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

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGameManager)))
	{
		saveGameManager->OnLoadGameData.AddUObject(this, &ThisClass::OnGameLoaded);
		if (!saveGameManager->HasSaveGame())
		{
			bool hasAllPrerequisites = HasAllPrequisites();
			HidePurchaseLocation(hasAllPrerequisites);
			if (!hasAllPrerequisites)
			{
				for (const FPrerequisiteUnlocks& prereq : _prerequisitesNeeded)
				{
					UPurchaseLocation* purchaseLocation = ensure(IsValid(prereq.PurchaseLocationActor)) ? prereq.PurchaseLocationActor->FindComponentByClass<UPurchaseLocation>() : nullptr;

					if (ensure(IsValid(purchaseLocation)))
					{
						purchaseLocation->OnUpgradePurchased.AddUObject(this, &UPurchaseLocation::OnPrerequisitePurchaseSuccess);
					}
				}
			}
		}
	}

	StaticOnPurchaseSuccess.AddUObject(this, &UPurchaseLocation::OnAnyPurchaseSuccess);
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

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGameManager))
	{
		saveGameManager->OnLoadGameData.RemoveAll(this);
	}

	StaticOnPurchaseSuccess.RemoveAll(this);

	for (const FPrerequisiteUnlocks& prerequisite : _prerequisitesNeeded)
	{
		if (ensure(IsValid(prerequisite.PurchaseLocationActor)))
		{
			UPurchaseLocation* purchaseLocation = prerequisite.PurchaseLocationActor->FindComponentByClass<UPurchaseLocation>();
			if (ensure(IsValid(purchaseLocation)))
			{
				purchaseLocation->OnUpgradePurchased.RemoveAll(this);
			}
		}
	}

	Super::EndPlay(endPlayReason);
}

void UPurchaseLocation::OnDayBegin()
{
	HidePurchaseLocation(true);
}

void UPurchaseLocation::OnDayEnd()
{
	if (_canPurchase && HasAllPrequisites())
	{
		HidePurchaseLocation(false);
	}
}

void UPurchaseLocation::HidePurchaseLocation(bool shouldHide)
{
	GetOwner()->SetActorHiddenInGame(shouldHide);
	GetOwner()->SetActorEnableCollision(!shouldHide);
}

void UPurchaseLocation::OnPrerequisitePurchaseSuccess()
{
	if (!_hasAllPrerequisites && HasAllPrequisites())
	{
		_hasAllPrerequisites = true;
		HidePurchaseLocation(false);
	}
}

void UPurchaseLocation::CheckPrerequisites()
{
	if (_currentPurchaseCount < _maxPurchaseCount)
	{
		bool hasAllPrerequisites = HasAllPrequisites();
		HidePurchaseLocation(!hasAllPrerequisites);
		if (!hasAllPrerequisites)
		{
			for (const FPrerequisiteUnlocks& prereq : _prerequisitesNeeded)
			{
				UPurchaseLocation* purchaseLocation = ensure(IsValid(prereq.PurchaseLocationActor)) ? prereq.PurchaseLocationActor->FindComponentByClass<UPurchaseLocation>() : nullptr;

				if (ensure(IsValid(purchaseLocation)))
				{
					purchaseLocation->OnUpgradePurchased.AddUObject(this, &UPurchaseLocation::OnPrerequisitePurchaseSuccess);
				}
			}
		}
	}
}

bool UPurchaseLocation::HasAllPrequisites() const
{
	bool hasAllPrerequisitesNeeded = true;
	for (const FPrerequisiteUnlocks& prereq : _prerequisitesNeeded)
	{
		UPurchaseLocation* purchaseLocation = ensure(IsValid(prereq.PurchaseLocationActor)) ? prereq.PurchaseLocationActor->FindComponentByClass<UPurchaseLocation>() : nullptr;
		if (!ensure(IsValid(purchaseLocation)) || purchaseLocation->GetCurrentPurchaseCount() < prereq.NeededLevel)
		{
			hasAllPrerequisitesNeeded = false;
			break;
		}
	}

	return hasAllPrerequisitesNeeded;
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
			StaticOnPurchaseSuccess.Broadcast();
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
	OnUpgradePurchased.Broadcast();

	if (_currentPurchaseCount >= _maxPurchaseCount)
	{
		SetCanPurchase(false);
		HidePurchaseLocation(true);
	}
}

void UPurchaseLocation::OnAnyPurchaseSuccess()
{
	Cosmetic_OnAnyUpgradePurchaseSuccess.Broadcast();
}

void UPurchaseLocation::OnGameLoaded(UFarmFPSSaveGame* saveGame)
{
	if (ensure(IsValid(saveGame)))
	{
		const FUpgradeLocationSaveData* upgradeLocationSaveData = saveGame->GetUpgradeLocationSaveDatas().FindByPredicate([this](const FUpgradeLocationSaveData& data)
			{
				return data.PurchaseLocationName == GetOwner()->GetName();
			});

		if (ensure(upgradeLocationSaveData))
		{
			SetCurrentPurchaseCountFromLoad(upgradeLocationSaveData->NumberOfPurchases);
		}
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPurchaseLocation::CheckPrerequisites);
}

void UPurchaseLocation::SetCurrentPurchaseCountFromLoad(int count)
{
	_currentPurchaseCount = count;
	for (int i = 0; i < _currentPurchaseCount; i++)
	{
		_purchaseCost *= _purchaseCostMultiplier;
	}
	if (_currentPurchaseCount >= _maxPurchaseCount)
	{
		SetCanPurchase(false);
		HidePurchaseLocation(true);
	}

	Cosmetic_OnPurchaseSuccess.Broadcast();
}

FUpgradeLocationSaveData UPurchaseLocation::GetUpgradeLocationSaveData() const
{
	FUpgradeLocationSaveData saveData;
	saveData.NumberOfPurchases = _currentPurchaseCount;
	saveData.PurchaseLocationName = GetOwner()->GetName();
	saveData.IsEnabled = GetCanPurchase();

	return saveData;
}

