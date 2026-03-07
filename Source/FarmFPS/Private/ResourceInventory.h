// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ResourceData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "ResourceInventory.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UResourceInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UResourceInventory();

	void AddResource(EResourceType resourceType, uint16 yieldAmount);
	void RemoveResource(EResourceType resourceType, uint16 yieldAmount);
	void SetResourceAmount(EResourceType resourceType, uint16 newAmount);

	void AddAllResourcesInInventory(UResourceInventory* otherInventory);

	bool CanAddResource(EResourceType resourceType, uint16 amount) const;

	UFUNCTION(BlueprintPure)
	int GetResourceCount(EResourceType resourceType) const;

	UFUNCTION(BlueprintPure)
	bool HasResourceAmount(EResourceType resourceType, int amount) const;

	uint16 GetResourceCap(EResourceType resourceType) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceCountChangedEvent, EResourceType, resourceType, int, newAmount);
	FOnResourceCountChangedEvent OnResourceCountChanged;

protected:
	virtual void BeginPlay() override;		

private:
	void CheckInitializeMap(EResourceType cropType);

	TMap<EResourceType, uint16> _resourcesMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<EResourceType, uint16> _resourceCaps;
};
