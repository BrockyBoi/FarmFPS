// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "AutomaticResourceTransferPoint.h"
#include "ModifiedValueData.h"
#include "ResourceInventory.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "InputOutputStationActor.generated.h"

UCLASS()
class AInputOutputStationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AInputOutputStationActor();

	UResourceInventory* GetInputInventory() const { return _inputInventory; }
	UResourceInventory* GetOutputInventory() const { return _outputInventory; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual float GetTimeBetweenSpawns() const;

	struct ResourcesToSpawnData
	{
		FGameplayTag ResourceType;
		int AmountToSpawn;
	};

	virtual void SpawnResource(ResourcesToSpawnData& data);

	UFUNCTION()
	virtual void OnInputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount);

	UFUNCTION()
	virtual void OnOutputInventoryResourceCountChanged(const FGameplayTag& resourceType, float amount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAutomaticResourceTransferPoint* _resourceInputPoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAutomaticResourceTransferPoint* _resourceOutputPoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceInventory* _inputInventory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceInventory* _outputInventory = nullptr;

	UPROPERTY(EditAnywhere)
	FVector _launchVector;

	float _currentSpawnTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _defaultTimeBetweenSpawns = .45f;

	TArray<ResourcesToSpawnData> _resourcesToSpawnFromInputInventory;

	UPROPERTY(EditDefaultsOnly)
	bool _outputPhysicalObjectsFromInputInventory = false;
};
