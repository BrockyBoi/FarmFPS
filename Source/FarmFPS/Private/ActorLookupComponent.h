// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ActorLookupComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UActorLookupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorLookupComponent();
	TSubclassOf<AActor> GetActorForType(const FGameplayTag& resourceType) const;

protected:
	virtual void BeginPlay() override;	
	void InitializeMapFromTables();

private:
	mutable TMap<FGameplayTag, TSubclassOf<AActor>> _actorTypeToActorMap;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _resourceActorTable;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* _projectileActorTable;
};
