// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "CraftingData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "CustomerSpawnerManager.generated.h"

class ACustomer;

namespace CustomerTypeTags
{
	static const FGameplayTag StandardCustomer = FGameplayTag::RequestGameplayTag(FName("CustomerType.Standard"));
	static const FGameplayTag GiantCustomer = FGameplayTag::RequestGameplayTag(FName("CustomerType.Giant"));
}

USTRUCT(BlueprintType)
struct FCustomerSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACustomer> CustomerClass;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue SpawnChance = 0.f;
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	const FGameplayTag& GetNextCustomerTypeToSpawn() const;
	const TSubclassOf<ACustomer> GetNextCustomerSpawnClass(const FGameplayTag& customerType);

	UFUNCTION()
	void OnDayBegin();
	
	UFUNCTION()
	void OnDayEnd();

	UFUNCTION()
	void AttemptSpawnCustomer();

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Rate")
	FModifiedFloatValue _spawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Amount")
	FModifiedIntValue _totalCustomersAllowedOnScreenAtOnce;

	UPROPERTY(EditDefaultsOnly, Category = "Customer Types")
	FCustomerSpawnData _defaultCustomerSpawnData;

	UPROPERTY(EditDefaultsOnly, Category = "Customer Types")
	FCustomerSpawnData _giantCustomerSpawnData;

	int _currentCustomersOnScreen = 0;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AActor* _customerSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	FVector _customerSpawnPoint;

	FTimerHandle _spawnTimer;
};
