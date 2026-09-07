// Fill out your copyright notice in the Description page of Project Settings.

#include "UnlockActorLocation.h"

AActor* UUnlockActorLocation::GetNextActorToUnlock() const
{
	if (_unlockActorLocationData.Num() < 0 || _currentPurchaseCount >= _unlockActorLocationData.Num())
	{
		return nullptr;
	}

	const FUnlockActorLocationData& unlockData = _unlockActorLocationData[_currentPurchaseCount];
	if (unlockData.ActorsInSceneToEnable.Num() <= 0)
	{
		return nullptr;
	}

	return unlockData.ActorsInSceneToEnable[0];
}

void UUnlockActorLocation::BeginPlay()
{
	Super::BeginPlay();

	for (int unlockIndex = 0; unlockIndex < _unlockActorLocationData.Num(); unlockIndex++)
	{
		UnlockActors(false, unlockIndex);
	}
}

void UUnlockActorLocation::OnPurchaseSuccess(UPerkManager* perkManager, UResourceInventory* inventory)
{
	UnlockActors(true);

	Super::OnPurchaseSuccess(perkManager, inventory);
}

void UUnlockActorLocation::UnlockActors(bool shouldUnlock)
{
	UnlockActors(shouldUnlock, _currentPurchaseCount);
}

void UUnlockActorLocation::UnlockActors(bool shouldUnlock, int index)
{
	if (_unlockActorLocationData.Num() < 0 || index >= _unlockActorLocationData.Num())
	{
		return;
	}

	const FUnlockActorLocationData& unlockData = _unlockActorLocationData[index];
	for (AActor* actor : unlockData.ActorsInSceneToEnable)
	{
		if (ensure(IsValid(actor)))
		{
			actor->SetActorHiddenInGame(!shouldUnlock);
			actor->SetActorEnableCollision(shouldUnlock);
		}
	}

	for (AActor* actor : unlockData.ActorsInSceneToDisable)
	{
		if (ensure(IsValid(actor)))
		{
			actor->SetActorHiddenInGame(shouldUnlock);
			actor->SetActorEnableCollision(!shouldUnlock);
		}
	}
}

void UUnlockActorLocation::SetCurrentPurchaseCountFromLoad(int count)
{
	Super::SetCurrentPurchaseCountFromLoad(count);

	for (int i = 0; i < count; i++)
	{
		UnlockActors(true, i);
	}
}
