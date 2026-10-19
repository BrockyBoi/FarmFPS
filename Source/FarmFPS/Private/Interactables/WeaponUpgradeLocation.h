// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PurchaseLocation.h"

// Shooter
#include "ShooterWeapon.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "WeaponUpgradeLocation.generated.h"

class AShooterWeapon;
enum class EWaponFireType : uint8;

// Don't know why I made a separate struct just for this.  Maybe there's a reason?  Keeping it for now I guess.
USTRUCT(BlueprintType)
struct FWeaponUpgradeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (Categories = "WeaponType."))
	FGameplayTag WeaponToUpgrade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponFireType FireType;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UWeaponUpgradeLocation : public UPurchaseLocation
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	FString GetPerkDescriptionString() const;

protected:
	virtual void OnPurchaseSuccess(class UPerkManager* perkManager, class UResourceInventory* inventory) override;

	AShooterWeapon* GetWeapon() const;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (Categories = "WeaponType."))
	FWeaponUpgradeData _weaponUpgradeData;

};
