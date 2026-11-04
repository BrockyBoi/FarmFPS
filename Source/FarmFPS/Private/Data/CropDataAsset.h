// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraComponent.h"

// Generated
#include "CropDataAsset.generated.h"

UCLASS()
class UCropDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara System")
	TSoftObjectPtr<UNiagaraSystem> SunVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara System")
	TSoftObjectPtr<UNiagaraSystem> WaterVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara System")
	TSoftObjectPtr<UNiagaraSystem> LoveVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Particles")
	TSoftObjectPtr<UParticleSystem> DestructionParticleSystem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Mesh")
	TArray<TSoftObjectPtr<UStaticMesh>> MeshesOverProgress;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Mesh")
	FLinearColor StartingDeadFoliageColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Material")
	TSoftObjectPtr<UMaterialInstance> StandardMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Material")
	TSoftObjectPtr<UMaterialInstance> FinalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Audio")
	TSoftObjectPtr<USoundBase> OnBreakCropSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Tunables")
	FCachedModifiableValueDataTableRowHandle NumberOfPickupsToDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Tunables")
	FCachedModifiableValueDataTableRowHandle NumberOfLoveSeedsToDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Categories = "ResourceType."))
	FGameplayTag ResourceType;
};
