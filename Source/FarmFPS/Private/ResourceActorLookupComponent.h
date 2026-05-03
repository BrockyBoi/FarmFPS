// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ResourceActorLookupComponent.generated.h"

class AResourcePickupActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UResourceActorLookupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UResourceActorLookupComponent();
	TSubclassOf<AResourcePickupActor> GetResourceActorForType(const FGameplayTag& resourceType) const;

protected:
	virtual void BeginPlay() override;	

private:
	mutable TMap<FGameplayTag, TSubclassOf<AResourcePickupActor>> _resourceTypeToActorMap;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _dataTable;
};
