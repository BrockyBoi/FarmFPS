// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "PlayerInventoryItemSelector.generated.h"

class UResourceInventory;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPlayerInventoryItemSelector : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerInventoryItemSelector();

	void IncreaseIndex();
	void DecreaseIndex();
	void SetIndexValue(int index);
	int GetCurrentIndex() const { return _currentSelectedIndex; }

	void SetPlayerInventory(UResourceInventory* playerInventory);

	UFUNCTION(BlueprintCallable)
	const FGameplayTag& GetItemTypeAtIndex(int index) const { return _currentInventoryItemTypes.IsValidIndex(index) ? _currentInventoryItemTypes[index] : FGameplayTag::EmptyTag; }

	UFUNCTION(BlueprintPure)
	const FGameplayTag& GetCurrentSelectedItemType() const { return _currentInventoryItemTypes.IsValidIndex(_currentSelectedIndex) ? _currentInventoryItemTypes[_currentSelectedIndex] : FGameplayTag::EmptyTag; }

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnIndexChangedEvent, int index, FGameplayTag resourceType);
	FOnIndexChangedEvent OnIndexChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnResourceChanged(const FGameplayTag& resourceType, float newAmount);
	void SetCurrentInventoryItemTypes();

	UPROPERTY(Transient)
	UResourceInventory* _playerInventory = nullptr;

	TArray<FGameplayTag> _currentInventoryItemTypes;

	int _currentSelectedIndex = -1;
};
