// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "MoonHitBox.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMoonHitBox : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMoonHitBox();

	void HitMoon();

protected:

private:
	void OnNightStart();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly)
	int _hitsNeeded = 5;

	int _hitsLeft = 0;
};
