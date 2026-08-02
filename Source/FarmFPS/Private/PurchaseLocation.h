// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "PurchaseLocation.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPurchaseLocation : public UActorComponent
{
	GENERATED_BODY()

public:
	UPurchaseLocation();

	void SetCanPurchase(bool canPurchase) { _canPurchase = canPurchase; }
	bool GetCanPurchase() const { return _canPurchase; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnDayBegin();
	virtual void OnDayEnd();

	virtual void HidePurchaseLocation(bool shouldHide);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPurchaseSuccess);

	UPROPERTY(BlueprintAssignable)
	FOnPurchaseSuccess Cosmetic_OnPurchaseSuccess;

	UFUNCTION()
	virtual void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool AttemptPurchase(class UPerkManager* perkManager, class UResourceInventory* inventory);
	virtual void OnPurchaseSuccess(class UPerkManager* perkManager, class UResourceInventory* inventory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* _overlappingComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Purchase")
	float _purchaseCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Purchase")
	float _purchaseCostMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Purchase")
	int _maxPurchaseCount = 5;

	int _currentPurchaseCount = 0;

	bool _canPurchase = true;
};
