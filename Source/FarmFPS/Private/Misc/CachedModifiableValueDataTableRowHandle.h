// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "Managers/BaseModifiableValueData.h"
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"

// Generated
#include "CachedModifiableValueDataTableRowHandle.generated.h"

USTRUCT(BlueprintType)
struct FARMFPS_API FCachedModifiableValueDataTableRowHandle
{
	GENERATED_BODY()

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

private:
	mutable FModifiedFloatValue CachedValue;

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle _rowHandle;

	UPROPERTY(EditAnywhere)
	float DefaultValue;

	mutable bool _isCachedValueSet = false;
};
