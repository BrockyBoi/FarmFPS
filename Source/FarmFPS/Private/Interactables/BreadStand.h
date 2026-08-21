// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"
#include "Interactables/InputOutputStationActor.h"

// UE
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "BreadStand.generated.h"

class UCustomerQueue;

USTRUCT(BlueprintType)
struct FBreadPropData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> BreadPropClass;

	UPROPERTY(EditAnywhere)
	USceneComponent* BreadSpawnLocationComponent;

	UPROPERTY(EditAnywhere)
	float BreadPropHeight;

	TArray<AActor*> BreadPropsInScene;
};

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

	UFUNCTION(BlueprintCallable)
	FBreadPropData& GetBreadPropData(const FGameplayTag& breadType);

	UFUNCTION(BlueprintCallable)
	void SetBreadPropDataSpawnLocation(const FGameplayTag& breadType, USceneComponent* sceneComponent);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount) override;
	void OnDayBegin();
	void OnDayEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCustomerQueue* _customerQueue;

	UPROPERTY(EditDefaultsOnly)
	TArray<FModifiedResourceValue> _priceDataArray;

	UPROPERTY(EditAnywhere)
	FModifiedFloatValue _bonusPriceMultiplierOnAllBreadSold = 2.f;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FBreadPropData> _breadPropSpawnDatas;

	bool _isCurrentlySellingBreadToCustomer = false;

	FBreadPropData _emptyData;
};
