// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "AutomaticResourceTransferPoint.generated.h"

class UPrimitiveComponent;
class UResourceInventory;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UAutomaticResourceTransferPoint : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutomaticResourceTransferPoint();

	void SetGivesResources(bool givesResources);
	void SetInventory(UResourceInventory* inventory);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite)
	UPrimitiveComponent* _playerCollider = nullptr;

	TWeakObjectPtr<UResourceInventory> _inventory = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."), meta = (EditCondition = "_givesResources == false", EditConditionHides))
	FGameplayTagContainer _resourcesAllowed;

	UPROPERTY(EditDefaultsOnly)
	bool _givesResources = false;
};
