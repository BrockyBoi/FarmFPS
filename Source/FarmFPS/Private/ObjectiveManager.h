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

	DECLARE_EVENT_TwoParams(UObjectiveManager, FOnObjectiveProgressUpdatedEvent, FObjectiveData&, int);
	DECLARE_EVENT_OneParam(UObjectiveManager, FOnObjectiveCompletedEvent, FObjectiveData&);

	FOnObjectiveProgressUpdatedEvent OnObjectiveProgressUpdated;
	FOnObjectiveCompletedEvent OnObjectiveCompleted;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	

	void RemoveObjective(FObjectiveData& objectiveData);

	TMap<FGameplayTag, TArray<FObjectiveData>> _objectivesMap;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	FObjectiveData _debugTestObjective;
};
