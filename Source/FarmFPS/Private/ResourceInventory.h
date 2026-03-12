// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ResourceData.h"

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

	void AddResource(const FGameplayTag& resourceType, uint16 yieldAmount);
	void RemoveResource(const FGameplayTag& resourceType, uint16 yieldAmount);
	void SetResourceAmount(const FGameplayTag& resourceType, uint16 newAmount);

	void AddAllResourcesInInventory(UResourceInventory* otherInventory);

	bool CanAddResource(const FGameplayTag& resourceType, uint16 amount) const;

	UFUNCTION(BlueprintPure)
	int GetResourceCount(const FGameplayTag& resourceType) const;

	UFUNCTION(BlueprintPure)
	bool HasResourceAmount(const FGameplayTag& resourceType, int amount) const;

	uint16 GetResourceCap(const FGameplayTag& resourceType) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceCountChangedEvent, const FGameplayTag&, resourceType, int, newAmount);
	FOnResourceCountChangedEvent OnResourceCountChanged;

protected:
	virtual void BeginPlay() override;		

private:
	void CheckInitializeMap(const FGameplayTag& cropType);

	TMap<FGameplayTag, uint16> _resourcesMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, uint16> _resourceCaps;
};
