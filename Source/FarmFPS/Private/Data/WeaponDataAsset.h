// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraComponent.h"

// Generated
#include "WeaponDataAsset.generated.h"

UCLASS()
class UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Mesh")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Mesh")
	TSoftObjectPtr<UMaterialInstance> MeshMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Animation")
	TSoftClassPtr<UAnimInstance> FirstPersonAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Animation")
	TSoftClassPtr<UAnimInstance> ThirdPersonAnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Data|Animation")
	TSoftObjectPtr<UAnimMontage> FiringMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Data|Animation")
	TSoftObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Data|Animation")
	TSoftObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Audio")
	TSoftObjectPtr<USoundBase> ShootSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Audio")
	TSoftObjectPtr<USoundBase> ReloadSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara System")
	TSoftObjectPtr<UNiagaraSystem> MuzzleFlashVFX;
};
