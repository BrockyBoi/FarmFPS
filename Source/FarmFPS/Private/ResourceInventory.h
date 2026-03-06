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

	void AddResource(EResourceType cropType, uint16 yieldAmount);
	void RemoveResource(EResourceType cropType, uint16 yieldAmount);

	void AddAllResourcesInInventory(UResourceInventory* otherInventory);

	bool CanAddResource(EResourceType cropType, uint16 amount) const;

	UFUNCTION(BlueprintPure)
	int GetResourceCount(EResourceType cropType) const;

	UFUNCTION(BlueprintPure)
	bool HasResourceAmount(EResourceType cropType, int amount) const;

	uint16 GetResourceCap(EResourceType resourceType) const;

protected:
	virtual void BeginPlay() override;		

private:
	void CheckInitializeMap(EResourceType cropType);

	TMap<EResourceType, uint16> _resourcesMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<EResourceType, uint16> _resourceCaps;
};
