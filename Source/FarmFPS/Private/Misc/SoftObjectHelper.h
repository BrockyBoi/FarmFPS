#pragma once

// Brock
#include "Managers/AudioManager.h"

// UE
#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DECLARE_DELEGATE_OneParam(FOnAsyncLoad, UObject* loadedObject);

class SoftObjectHelper
{

public:
	template<typename T>
	static T* SyncLoadSoftObject(TSoftObjectPtr<T> softPointer)
	{
		if (ensure(!softPointer.IsNull()))
		{
			if (softPointer.IsValid())
			{
				return softPointer.Get();
			}

			return softPointer.LoadSynchronous();
		}

		return nullptr;
	}

	template<typename T>
	static void AsyncLoadSoftObject(UObject* WorldContextObject, TSoftObjectPtr<T> softPointer, FOnAsyncLoad callback)
	{
		if (ensure(!softPointer.IsNull()))
		{
			if (softPointer.IsValid())
			{
				callback.Execute(softPointer.Get());
				return;
			}

			softPointer.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(WorldContextObject, [callback](const FSoftObjectPath& path, UObject* loadedObject)
				{
					if (T* obj = Cast<T>(loadedObject); ensure(IsValid(obj)))
					{
						callback.Execute(obj);
					}
				}));
		}
	}

	template<typename T>
	static void AsyncLoadSoftObjectNoCallback(UObject* WorldContextObject, TSoftObjectPtr<T> softPointer)
	{
		if (ensure(!softPointer.IsNull()) && !softPointer.IsValid())
		{
			softPointer.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(WorldContextObject,[](const FSoftObjectPath&, UObject*){}));
		}
	}

	template<typename T>
	static void AsyncLoadSoftClass(UObject* WorldContextObject, TSoftClassPtr<T> softPointer, FOnAsyncLoad callback)
	{
		if (ensure(!softPointer.IsNull()))
		{
			if (softPointer.IsValid())
			{
				callback.Execute(softPointer.Get());
				return;
			}

			FSoftObjectPath path = softPointer.ToSoftObjectPath();
			FStreamableManager& streamable = UAssetManager::GetStreamableManager();
			streamable.RequestAsyncLoad(path, FStreamableDelegate::CreateWeakLambda(WorldContextObject, [callback, softPointer]()
				{
					if (T* obj = Cast<T>(softPointer.Get()); ensure(IsValid(obj)))
					{
						callback.Execute(obj);
					}
				}));
		}
	}

	template<typename T>
	static void AsyncLoadSoftClassNoCallback(UObject* WorldContextObject, TSoftClassPtr<T> softPointer)
	{
		if (ensure(!softPointer.IsNull()) && !softPointer.IsValid())
		{
			FSoftObjectPath path = softPointer.ToSoftObjectPath();
			FStreamableManager& streamable = UAssetManager::GetStreamableManager();
			streamable.RequestAsyncLoad(path, FStreamableDelegate::CreateWeakLambda(WorldContextObject, [](){}));
		}
	}

	static void SetStaticMesh(UObject* WorldContextObject, TSoftObjectPtr<UStaticMesh> softPointer, UStaticMeshComponent* staticMeshComponent)
	{
		if (!ensure(IsValid(staticMeshComponent)))
		{
			return;
		}

		AsyncLoadSoftObject(WorldContextObject, softPointer, FOnAsyncLoad::CreateWeakLambda(WorldContextObject, [staticMeshComponent](UObject* meshPointer)
			{
				if (UStaticMesh* mesh = Cast<UStaticMesh>(meshPointer); ensure(IsValid(mesh)))
				{
					staticMeshComponent->SetStaticMesh(mesh);
				}
			}));
	}

	static void SetStaticMeshMaterial(UObject* WorldContextObject, TSoftObjectPtr<UMaterialInstance> softPointer, UStaticMeshComponent* staticMeshComponent)
	{
		if (!ensure(IsValid(staticMeshComponent)))
		{
			return;
		}

		AsyncLoadSoftObject(WorldContextObject, softPointer, FOnAsyncLoad::CreateWeakLambda(WorldContextObject, [staticMeshComponent](UObject* materialPointer)
			{
				if (UMaterialInstance* material = Cast<UMaterialInstance>(materialPointer); ensure(IsValid(material)))
				{
					staticMeshComponent->SetMaterial(0, material);
				}
			}));
	}

	static void SetStaticMeshMaterialOverlay(UObject* WorldContextObject, TSoftObjectPtr<UMaterialInstance> softPointer, UStaticMeshComponent* staticMeshComponent)
	{
		if (!ensure(IsValid(staticMeshComponent)))
		{
			return;
		}

		AsyncLoadSoftObject(WorldContextObject, softPointer, FOnAsyncLoad::CreateWeakLambda(WorldContextObject, [staticMeshComponent](UObject* materialPointer)
			{
				if (UMaterialInstance* material = Cast<UMaterialInstance>(materialPointer); ensure(IsValid(material)))
				{
					staticMeshComponent->SetOverlayMaterial(material);
				}
			}));
	}

	static void PlaySpawnedSoundAtLocation(UObject* WorldContextObject, TSoftObjectPtr<USoundBase> softPointer, const FVector& location)
	{
		AsyncLoadSoftObject(WorldContextObject, softPointer, FOnAsyncLoad::CreateWeakLambda(WorldContextObject, [WorldContextObject, location](UObject* soundClip)
			{
				if (USoundBase* sound = Cast<USoundBase>(soundClip); ensure(IsValid(sound)))
				{
					UAudioManager::SpawnSoundAtLocation(WorldContextObject, sound, location);
				}
			}));
	}

	static void PlaySpawned2DSound(UObject* WorldContextObject, TSoftObjectPtr<USoundBase> softPointer)
	{
		AsyncLoadSoftObject(WorldContextObject, softPointer, FOnAsyncLoad::CreateWeakLambda(WorldContextObject, [WorldContextObject](UObject* soundClip)
			{
				if (USoundBase* sound = Cast<USoundBase>(soundClip); ensure(IsValid(sound)))
				{
					UAudioManager::SpawnSound2D(WorldContextObject, sound);
				}
			}));
	}

	static UNiagaraComponent* SpawnAttachedNiagaraComponent(UObject* WorldContextObject, USceneComponent* sceneComponent, UObject* niagaraObject, const FVector& location, const FRotator& rotation, const FVector& scale, const bool autoDestroy, const bool autoActivate)
	{
		if (UNiagaraSystem* niagara = Cast<UNiagaraSystem>(niagaraObject); ensure(IsValid(niagara)))
		{
			const FName attachName = NAME_None;
			if (UNiagaraComponent* niagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(niagara, sceneComponent, attachName, location, rotation, EAttachLocation::Type::SnapToTargetIncludingScale, autoDestroy); ensure(IsValid(niagaraComponent)))
			{
				return niagaraComponent;
			}
		}

		return nullptr;
	}

	static UParticleSystemComponent* SpawnParticleSystemComponent(UObject* WorldContextObject, UObject* particleSystem, const FVector& location, const FRotator& rotation, const FVector& scale, const bool autoDestroy, const bool autoActivate)
	{
		if (UParticleSystem* particles = Cast<UParticleSystem>(particleSystem); ensure(IsValid(particles)))
		{
			if (UParticleSystemComponent* particleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject, particles, location, rotation, scale, autoDestroy); ensure(IsValid(particleSystemComponent)))
			{
				return particleSystemComponent;
			}
		}

		return nullptr;
	}
};

