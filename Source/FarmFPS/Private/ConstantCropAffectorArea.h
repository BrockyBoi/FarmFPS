// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Brock
#include "ModifiedValueData.h"

// UE
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "ConstantCropAffectorArea.generated.h"

class UCropComponent;

UCLASS()
class AConstantCropAffectorArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AConstantCropAffectorArea();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnLifeTimeEnd();

	UFUNCTION()
	void OnCropDestroyed(UCropComponent* cropDestroyed);

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* _cropAffectorArea;

	TArray<TWeakObjectPtr<UCropComponent>> _cropsInArea;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "CropResourceType."))
	FGameplayTag _cropResourceType;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _resourceAmountPerSecond = 1.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _lifetime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _affectorRadius = 40.f;

	UPROPERTY(EditDefaultsOnly)
	FModifiedFloatValue _affectorHeight = 20.f;
};
