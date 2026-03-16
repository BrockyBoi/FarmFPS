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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMFPS_API UCustomerSpawnerManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomerSpawnerManager();
	void OnCustomerLeaveMap();

	bool IsRoomForNewCustomer() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void AttemptSpawnCustomer();

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Rate")
	FModifiedFloatValue _spawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Amount")
	FModifiedIntValue _totalCustomersAllowedOnScreenAtOnce;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACustomer> _customerToSpawn;

	int _currentCustomersOnScreen = 0;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AActor* _customerSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	FVector _customerSpawnPoint;

	FTimerHandle _spawnTimer;
};
