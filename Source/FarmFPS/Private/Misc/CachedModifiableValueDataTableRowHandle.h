// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Data/BaseModifiableValueData.h"
#include "Data/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "CachedModifiableValueDataTableRowHandle.generated.h"

DECLARE_STATS_GROUP(TEXT("ModifiedValueSystem"), STATGROUP_ModifiedValues, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("GetModifiedValue"), STAT_GetModifiedValue, STATGROUP_ModifiedValues);

USTRUCT(BlueprintType)
struct FARMFPS_API FCachedModifiableValueDataTableRowHandle
{
	GENERATED_BODY()

	FCachedModifiableValueDataTableRowHandle()
	{

	}

	FCachedModifiableValueDataTableRowHandle(float value)
	{
		DefaultValue = value;
	}

	FModifiedFloatValue& GetValue() const
	{
		if (!_isCachedValueSet)
		{
			if (ensure(!_rowHandle.IsNull()))
			{
				FBaseModifiableValueData* row = _rowHandle.GetRow<FBaseModifiableValueData>(TEXT("Looking shit up"));
				if (ensure(row != nullptr))
				{
					CachedValue = row->BaseModifiedValue;
				}
				else
				{
					CachedValue = FModifiedFloatValue(DefaultValue);
				}
			}
			else
			{
				CachedValue = FModifiedFloatValue(DefaultValue);
			}

			_isCachedValueSet = true;
		}

		return CachedValue;
	}

	float GetModifiedValue(const UObject* worldObject) const
	{
		SCOPE_CYCLE_COUNTER(STAT_GetModifiedValue);

		FModifiedFloatValue& value = GetValue();
		return value.GetModifiedValue(worldObject);
	}

private:
	mutable FModifiedFloatValue CachedValue;

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle _rowHandle;

	UPROPERTY(EditAnywhere)
	float DefaultValue;

	mutable bool _isCachedValueSet = false;
};
