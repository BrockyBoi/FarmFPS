// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "AutomaticResourceTransferPoint.h"
#include "CraftingData.h"
#include "InputOutputStationActor.h"
#include "ResourceInventory.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

// Generated
#include "BreadStand.generated.h"

class UCustomerQueue;

UCLASS()
class ABreadStand : public AInputOutputStationActor
{
	GENERATED_BODY()
	
public:	
	ABreadStand();
	const FVector GetNextCustomerQueuePosition();

	UCustomerQueue* GetCustomerQueue() const { return _customerQueue; }
	const FModifiedResourceValue GetPriceForResource(const FGameplayTag& resourceType) const;

	void SetIsCurrentlySellingBreadToCustomer(bool isSelling) { _isCurrentlySellingBreadToCustomer = isSelling; }
	bool GetIsCurrentlySellingBreadToCustomer() const { return _isCurrentlySellingBreadToCustomer; }

protected:
	virtual void BeginPlay() override;

	virtual void OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, int amount) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCustomerQueue* _customerQueue;

	UPROPERTY(EditDefaultsOnly)
	TArray<FModifiedResourceValue> _priceDataArray;

	bool _isCurrentlySellingBreadToCustomer = false;
};
