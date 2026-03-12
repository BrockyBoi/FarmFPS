// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PerkData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "UpgradePurchaceLocation.generated.h"

#define PerkModifierCategory meta = (Categories = "PerkModifier.")

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UUpgradePurchaceLocation : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUpgradePurchaceLocation();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool AttemptUpgradePurchase(class UPerkManager* perkManager, class UResourceInventory* inventory);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade", PerkModifierCategory)
	FGameplayTag _modifierUpgrade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade", PerkModifierCategory)
	float _upgradeCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	float _upgradeCostMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	FPerkData _perkUpgradeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* _overlappingComponent;
};
