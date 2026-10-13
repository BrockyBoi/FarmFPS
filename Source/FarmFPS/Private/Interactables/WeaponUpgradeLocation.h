// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Interactables/PurchaseLocation.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "WeaponUpgradeLocation.generated.h"

USTRUCT(BlueprintType)
struct FWeaponUpgradeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (Categories = "WeaponType."))
	FGameplayTag WeaponToUpgrade;

	bool MakeShotgun;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UWeaponUpgradeLocation : public UPurchaseLocation
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	FString GetPerkDescriptionString() const;

	virtual void OnPurchaseSuccess(class UPerkManager* perkManager, class UResourceInventory* inventory) override;

	UPROPERTY(EditAnywhere)
	FWeaponUpgradeData _weaponUpgradeData;
};
