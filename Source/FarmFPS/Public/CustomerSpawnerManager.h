// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Misc/CachedModifiableValueDataTableRowHandle.h"
#include "Managers/CraftingData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "CustomerSpawnerManager.generated.h"

class ACustomer;

USTRUCT(BlueprintType)
struct FCustomerSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACustomer> CustomerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Tunables")
	FCachedModifiableValueDataTableRowHandle SpawnChance = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMFPS_API UCustomerSpawnerManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomerSpawnerManager();
	void OnCustomerLeaveMap();

	bool IsRoomForNewCustomer() const;
	bool IsSpawnTimerActive() const;

	void SpawnCustomer_CHEAT();
	void SpawnGiantCustomer_CHEAT();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	const FGameplayTag GetNextCustomerTypeToSpawn() const;
	const TSubclassOf<ACustomer> GetNextCustomerSpawnClass(const FGameplayTag& customerType);

	UFUNCTION()
	void OnDayBegin();
	
	UFUNCTION()
	void OnDayEnd();

	UFUNCTION()
	void AttemptSpawnCustomer();

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Rate")
	FModifiedFloatValue _spawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Tunables|Spawn Amount")
	FCachedModifiableValueDataTableRowHandle _totalCustomersAllowedOnScreenAtOnce = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Customer Types")
	FCustomerSpawnData _defaultCustomerSpawnData;

	UPROPERTY(EditDefaultsOnly, Category = "Customer Types")
	FCustomerSpawnData _giantCustomerSpawnData;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer _allowedBreadTypesDesired;

	int _currentCustomersOnScreen = 0;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AActor* _customerSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	FVector _customerSpawnPoint;

	FTimerHandle _spawnTimer;
};
