// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ObjectiveData.h"

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

// Generated
#include "ObjectiveManager.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UObjectiveManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObjectiveManager();

	void IncrementObjectiveProgress(const FGameplayTag& objectiveTypeTag, const FGameplayTag& objectiveGoalTag, int amount = 1);
	void AddObjective(FObjectiveData& objectiveData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveProgressUpdatedEvent, FObjectiveData, objectiveData, int, newProgressCount);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompletedEvent, FObjectiveData, objectiveData);

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveProgressUpdatedEvent OnObjectiveProgressUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveCompletedEvent OnObjectiveCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveCompletedEvent OnMainObjectiveCompleted;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	

	void RemoveObjective(FObjectiveData& objectiveData);

	TMap<FGameplayTag, TArray<FObjectiveData>> _objectivesMap;

	UPROPERTY(EditDefaultsOnly)
	TArray<FObjectiveData> _mainObjectives;
	int _currentMainObjectiveIndex = 0;

	FObjectiveData _currentMainObjective;
};
