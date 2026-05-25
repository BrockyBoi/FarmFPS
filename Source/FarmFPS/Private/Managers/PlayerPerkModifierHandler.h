// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "PlayerPerkModifierHandler.generated.h"

class AShooterCharacter;
class UPerkManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPlayerPerkModifierHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerPerkModifierHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPerkLevelChange(const FGameplayTag& perkTag, const FPerkData& perkData);

	UPROPERTY(BlueprintReadWrite)
	USphereComponent* _playerPickupCollider = nullptr;

	UPROPERTY(Transient)
	AShooterCharacter* _owningCharacter = nullptr;

	UPROPERTY(Transient)
	UPerkManager* _perkManager = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue PlayerPickupColliderSizeModifier;
};
