// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectiveManager.h"

UObjectiveManager::UObjectiveManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UObjectiveManager::BeginPlay()
{
	Super::BeginPlay();

	_currentMainObjective = _mainObjectives.IsValidIndex(_currentMainObjectiveIndex) ? _mainObjectives[_currentMainObjectiveIndex] : FObjectiveData();
	AddObjective(_currentMainObjective);
}

void UObjectiveManager::IncrementObjectiveProgress(const FGameplayTag& objectiveTypeTag, const FGameplayTag& objectiveGoalTag, int amount)
{
	if (_objectivesMap.Contains(objectiveTypeTag))
	{
		TArray<FObjectiveData> objectivesToRemove;
		for (FObjectiveData& objectiveData : _objectivesMap[objectiveTypeTag])
		{
			if (objectiveData.GetObjectiveTypeTag() == objectiveTypeTag && objectiveData.GetObjectiveGoalTags().HasTag(objectiveGoalTag))
			{
				objectiveData.IncrementProgress(amount);

				OnObjectiveProgressUpdated.Broadcast(objectiveData, objectiveData.GetProgressCount());

				if (objectiveData.IsCompleted())
				{
					OnObjectiveCompleted.Broadcast(objectiveData);

					objectivesToRemove.Add(objectiveData);

					if (objectiveData == _currentMainObjective)
					{
						OnMainObjectiveCompleted.Broadcast(objectiveData);

						_currentMainObjectiveIndex++;
						if (_mainObjectives.IsValidIndex(_currentMainObjectiveIndex))
						{
							_currentMainObjective = _mainObjectives[_currentMainObjectiveIndex];
							AddObjective(_currentMainObjective);
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("No more main objectives left in the list!"));
						}
					}
				}
			}
		}

		for (FObjectiveData& objectiveData : objectivesToRemove)
		{
			RemoveObjective(objectiveData);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to increment progress for an objective that doesn't exist in the map! ObjectiveType: %s, ObjectiveGoal: %s"), *objectiveTypeTag.ToString(), *objectiveGoalTag.ToString());
	}
}

void UObjectiveManager::AddObjective(FObjectiveData& objectiveData)
{
	if (!_objectivesMap.Contains(objectiveData.GetObjectiveTypeTag()))
	{
		_objectivesMap.Add(objectiveData.GetObjectiveTypeTag(), TArray<FObjectiveData>());
	}
	_objectivesMap[objectiveData.GetObjectiveTypeTag()].Add(objectiveData);
}

void UObjectiveManager::RemoveObjective(FObjectiveData& objectiveData)
{
	if (_objectivesMap.Contains(objectiveData.GetObjectiveTypeTag()))
	{
		_objectivesMap[objectiveData.GetObjectiveTypeTag()].Remove(objectiveData);
	}
}
