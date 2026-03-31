// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ResourceInventory.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UResourceInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UResourceInventory();

	UFUNCTION(BlueprintCallable)
	void AddResource(const FGameplayTag& resourceType, float amount);

	void SetResourceCap(const FGameplayTag& resourceType, float cap);

	UFUNCTION(BlueprintCallable)
	void RemoveResource(const FGameplayTag& resourceType, float amount);
	void SetResourceAmount(const FGameplayTag& resourceType, float amount);

	void AddAllResourcesInInventory(UResourceInventory* otherInventory);

	bool CanAddResource(const FGameplayTag& resourceType, float amount) const;

	UFUNCTION(BlueprintPure)
	float GetResourceCount(const FGameplayTag& resourceType) const;

	UFUNCTION(BlueprintPure)
	bool HasResourceAmount(const FGameplayTag& resourceType, float amount) const;

	uint16 GetResourceCap(const FGameplayTag& resourceType) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceCountChangedEvent, const FGameplayTag&, resourceType, float, newAmount);
	
	UPROPERTY(BlueprintAssignable)
	FOnResourceCountChangedEvent OnResourceCountChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnDayEnd();

	void CheckInitializeMap(const FGameplayTag& cropType);
	void ClearAllExceptMoney();

	TMap<FGameplayTag, float> _resourcesMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, float> _resourceCaps;
};
