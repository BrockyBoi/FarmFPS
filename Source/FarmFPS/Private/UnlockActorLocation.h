// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "PurchaseLocation.h"

// UE 
#include "CoreMinimal.h"

// Generated
#include "UnlockActorLocation.generated.h"

USTRUCT(BlueprintType)
struct FUnlockActorLocationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsInSceneToEnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsInSceneToDisable;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UUnlockActorLocation : public UPurchaseLocation
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void OnPurchaseSuccess(class UPerkManager* perkManager, class UResourceInventory* inventory) override;

	void UnlockActors(bool shouldUnlock);
	void UnlockActors(bool shouldUnlock, int index);

	UPROPERTY(EditAnywhere)
	TArray<FUnlockActorLocationData> _unlockActorLocationData;

	int _currentUnlockIndex = 0;
};
