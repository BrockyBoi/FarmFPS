// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterProjectile.h"

// Brock
#include "Managers/ActorPool.h"
#include "Managers/AudioManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Misc/SoftObjectHelper.h"

#include "ShooterWeapon.h"

// UE
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "TimerManager.h"

AShooterProjectile::AShooterProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// create the collision component and assign it as the root
	RootComponent = CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));

	CollisionComponent->SetSphereRadius(16.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	// create the projectile movement component. No need to attach it because it's not a Scene Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bShouldBounce = true;

	// set the default damage type
	HitDamageType = UDamageType::StaticClass();
}

void AShooterProjectile::AddActorToPool()
{
	StopProjectile();

	Cosmetic_OnAddedToPool();
}

void AShooterProjectile::RemoveFromPool()
{
	_staticMeshComponent = FindComponentByClass<UStaticMeshComponent>();
	CollisionComponent->SetSimulatePhysics(true);

	if (ProjectileMovement)
	{
		//Shoot();
	}

	GetWorld()->GetTimerManager().SetTimer(LifeTimeTimer, this, &AShooterProjectile::OnLifeTimeExpired, LifeTime, false);
	Cosmetic_OnRemovedFromPool();

	if (ensure(IsValid(_staticMeshComponent)))
	{
		_staticMeshComponent->SetVisibility(true);

		if (ensure(IsValid(_projectileDataAsset)))
		{
			SoftObjectHelper::SetStaticMesh(_projectileDataAsset->Mesh, _staticMeshComponent);
			SoftObjectHelper::AsyncLoadSoftObject(_projectileDataAsset->TrailVFX, FOnAsyncLoad::CreateLambda([this](UObject* niagaraSystem)
				{
					if (!IsValid(_trailVFX))
					{
						_trailVFX = SoftObjectHelper::SpawnAttachedNiagaraComponent(this, GetRootComponent(), niagaraSystem, GetActorLocation(), GetActorRotation(), FVector::OneVector, false, false);
						_trailVFX->RegisterComponent();
					}

					if (ensure(IsValid(_trailVFX)))
					{
						//_trailVFX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
						_trailVFX->SetRelativeLocation(FVector::ZeroVector);
						_trailVFX->Activate();
					}
				}));

			if (!_projectileDataAsset->MeshMaterial.IsNull())
			{
				SoftObjectHelper::AsyncLoadSoftObject(_projectileDataAsset->MeshMaterial, FOnAsyncLoad::CreateLambda([&](UObject* materialInstance)
					{
						if (UMaterialInstance* material = Cast<UMaterialInstance>(materialInstance); ensure(IsValid(material)))
						{
							_staticMeshComponent->SetMaterial(0, material);
						}
					}));
			}
		}
	}
}

void AShooterProjectile::Shoot()
{
	CollisionComponent->SetSimulatePhysics(false);

	ProjectileMovement->SetUpdatedComponent(GetRootComponent());
	ProjectileMovement->Activate(true);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	ProjectileMovement->UpdateComponentVelocity();
}

void AShooterProjectile::ModifyThroughChargingWeapon(float percentageCharged)
{
	SetActorScale3D(FVector::OneVector * ScaleMultiplierAtMaxCharge * percentageCharged);
}

void AShooterProjectile::SetOwningWeapon(AShooterWeapon* weapon)
{
	_owningWeapon = TWeakObjectPtr<AShooterWeapon>(weapon);
}

void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// ignore the pawn that shot this projectile
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AShooterProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	// clear the destruction timer
	GetWorld()->GetTimerManager().ClearTimer(DestructionTimer);
	
	Super::EndPlay(EndPlayReason);
}

void AShooterProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// ignore if we've already hit something else
	if (bHit)
	{
		return;
	}

	bHit = true;

	// disable collision on the projectile
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// make AI perception noise
	MakeNoise(NoiseLoudness, GetInstigator(), GetActorLocation(), NoiseRange, NoiseTag);

	if (bExplodeOnHit)
	{
		// apply explosion damage centered on the projectile
		ExplosionCheck(GetActorLocation());
	} 
	else 
	{
		// single hit projectile. Process the collided actor
		ProcessHit(Other, OtherComp, Hit.ImpactPoint, -Hit.ImpactNormal);
	}

	// pass control to BP for any extra effects
	BP_OnProjectileHit(Hit);

	Cosmetic_OnHitObject();

	StopProjectile();

	// check if we should schedule deferred destruction of the projectile
	if (DeferredDestructionTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &AShooterProjectile::OnDeferredDestruction, DeferredDestructionTime, false);
	} 
	else 
	{
		UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this);
		if (ensure(IsValid(actorPool)))
		{
			actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
		}
	}
}

void AShooterProjectile::ExplosionCheck(const FVector& ExplosionCenter)
{
	// do a sphere overlap check look for nearby actors to damage
	TArray<FOverlapResult> Overlaps;

	FCollisionShape OverlapShape;
	OverlapShape.SetSphere(ExplosionRadius);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (!bDamageOwner)
	{
		QueryParams.AddIgnoredActor(GetInstigator());
	}

	GetWorld()->OverlapMultiByObjectType(Overlaps, ExplosionCenter, FQuat::Identity, ObjectParams, OverlapShape, QueryParams);

	TArray<AActor*> DamagedActors;

	// process the overlap results
	for (const FOverlapResult& CurrentOverlap : Overlaps)
	{
		// overlaps may return the same actor multiple times per each component overlapped
		// ensure we only damage each actor once by adding it to a damaged list
		if (DamagedActors.Find(CurrentOverlap.GetActor()) == INDEX_NONE)
		{
			DamagedActors.Add(CurrentOverlap.GetActor());

			// apply physics force away from the explosion
			const FVector& ExplosionDir = CurrentOverlap.GetActor()->GetActorLocation() - GetActorLocation();

			// push and/or damage the overlapped actor
			ProcessHit(CurrentOverlap.GetActor(), CurrentOverlap.GetComponent(), GetActorLocation(), ExplosionDir.GetSafeNormal());
		}
			
	}
}

void AShooterProjectile::ProcessHit(AActor* HitActor, UPrimitiveComponent* HitComp, const FVector& HitLocation, const FVector& HitDirection)
{
	// have we hit a character?
	if (ACharacter* HitCharacter = Cast<ACharacter>(HitActor))
	{
		// ignore the owner of this projectile
		if (HitCharacter != GetOwner() || bDamageOwner)
		{
			// apply damage to the character
			UGameplayStatics::ApplyDamage(HitCharacter, HitDamage, GetInstigator()->GetController(), this, HitDamageType);
		}
	}

	// have we hit a physics object?
	if (HitComp->IsSimulatingPhysics())
	{
		// give some physics impulse to the object
		HitComp->AddImpulseAtLocation(HitDirection * PhysicsForce, HitLocation);
	}
}

void AShooterProjectile::OnLifeTimeExpired()
{
	UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
	}
}

void AShooterProjectile::OnDeferredDestruction()
{
	if (UActorPool* actorPool = UFarmFPSUtilities::GetActorPool(this); ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(ProjectileType, this, EPooledActorType::Projectile);
	}
}

void AShooterProjectile::StopProjectile()
{
	if (CollisionComponent)
	{
		CollisionComponent->SetSimulatePhysics(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}

	SetActorScale3D(FVector::OneVector);
	GetWorld()->GetTimerManager().ClearTimer(LifeTimeTimer);

	if (IsValid(_staticMeshComponent))
	{
		_staticMeshComponent->SetVisibility(false);
	}

	if (IsValid(_trailVFX))
	{
		_trailVFX->Deactivate();
	}

	SetActorHiddenInGame(true);
}

void AShooterProjectile::SpawnAsyncOnProjectileHitVFX(float scale)
{
	if (!ensure(IsValid(_projectileDataAsset)))
	{
		return;
	}

	SoftObjectHelper::AsyncLoadSoftObject(_projectileDataAsset->OnHitVFX, FOnAsyncLoad::CreateLambda([this, scale](UObject* particleSystem)
		{
			if (!IsValid(_onHitVFX))
			{
				constexpr bool autoDestroy = false;
				constexpr bool autoActivate = false;
				_onHitVFX = SoftObjectHelper::SpawnParticleSystemComponent(this, particleSystem, GetActorLocation(), GetActorRotation(), FVector::OneVector * scale, autoDestroy, autoActivate);
			}

			if (ensure(IsValid(_onHitVFX)))
			{
				_onHitVFX->SetWorldLocation(GetActorLocation());
				_onHitVFX->DeactivateImmediate();
				_onHitVFX->Activate();
			}
		}));
}

void AShooterProjectile::PlayOnHitSound()
{
	if (ensure(IsValid(_projectileDataAsset)) && !_projectileDataAsset->OnHitSound.IsNull())
	{
		SoftObjectHelper::PlaySpawnedSoundAtLocation(this, _projectileDataAsset->OnHitSound, GetActorLocation());
	}
}
