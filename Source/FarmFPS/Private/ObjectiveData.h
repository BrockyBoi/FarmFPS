// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generteated
#include "ObjectiveData.generated.h"

USTRUCT(BlueprintType)
struct FObjectiveData
{
	GENERATED_BODY()

public:
	void IncrementProgress(int amount = 1)
	{
		_objectiveProgressCount = FMath::Clamp(_objectiveProgressCount + amount, 0, _objectiveProgressTarget);
	}

	float GetObjectivePercentComplete() const
	{
		if (_objectiveProgressTarget == 0)
		{
			return 0.f;
		}
		return FMath::Clamp(static_cast<float>(_objectiveProgressCount) / static_cast<float>(_objectiveProgressTarget), 0.f, 1.f);
	}

	bool IsCompleted() const 
	{
		return _objectiveProgressCount >= _objectiveProgressTarget;
	}

	int GetProgressCount() const
	{
		return _objectiveProgressCount;
	}

	int GetProgressTarget() const
	{
		return _objectiveProgressTarget;
	}

	bool GetIsMainObjective() const
	{
		return _isMainObjective;
	}

	const FGameplayTag& GetObjectiveTypeTag() const
	{
		return _objectiveTypeTag;
	}

	const FGameplayTagContainer& GetObjectiveGoalTags() const
	{
		return _objectiveGoalTags;
	}

	bool operator==(const FObjectiveData& other) const
	{
		return _objectiveTypeTag == other._objectiveTypeTag && _objectiveGoalTags == other._objectiveGoalTags;
	}

	bool operator!=(const FObjectiveData& other) const
	{
		return !(*this == other);
	}

protected:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ObjectiveType."), BlueprintReadOnly)
	FGameplayTag _objectiveTypeTag;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ResourceType."), BlueprintReadOnly)
	FGameplayTagContainer _objectiveGoalTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int _objectiveProgressTarget = 0;

	UPROPERTY(BlueprintReadOnly)
	int _objectiveProgressCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool _isMainObjective = false;
};