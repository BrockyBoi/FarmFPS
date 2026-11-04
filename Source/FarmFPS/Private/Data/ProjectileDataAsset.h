// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraComponent.h"

// Generated
#include "ProjectileDataAsset.generated.h"

UCLASS()
class UProjectileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Mesh")
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Mesh")
	TSoftObjectPtr<UMaterialInstance> MeshMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Audio")
	TSoftObjectPtr<USoundBase> OnHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara System")
	TSoftObjectPtr<UNiagaraSystem> TrailVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara System")
	TSoftObjectPtr<UParticleSystem> OnHitVFX;
};
