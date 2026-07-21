// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourcePickupActor.h"

// Modules
#include "FarmFPSCharacter.h"

//Brock
#include "Managers/ActorPool.h"
#include "Interactables/AutomaticResourceTransferPoint.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Managers/ObjectiveManager.h"
#include "Managers/ObjectiveTypeTags.h"
#include "Resources/ResourceInventory.h"

// UE
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AResourcePickupActor::AResourcePickupActor()
{
	PrimaryActorTick.bCanEverTick = true;

	_capsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	SetRootComponent(_capsuleCollider);

	_staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	_staticMesh->SetupAttachment(RootComponent);
}

void AResourcePickupActor::BeginPlay()
{
	Super::BeginPlay();
}

void AResourcePickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_isMovingToActor && ensure(_actorToMoveTo.IsValid()) && ensure(_inventoryOfActorMovingTowards.IsValid()))
	{
		FVector endLocation = _customEndLocation != FVector::ZeroVector ? _customEndLocation : _actorToMoveTo->GetActorLocation();
		SetActorLocation(FMath::Lerp(_startingMovementLocation, endLocation, _timeMovedToActor / _timeToMoveToActor));
		_timeMovedToActor += DeltaTime;

		if (_timeMovedToActor >= _timeToMoveToActor)
		{
			AddResourcesToInventory(_inventoryOfActorMovingTowards.Get());

			UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
			if (ensure(IsValid(actorPool)))
			{
				actorPool->AddActorToPool(_resourceType, this, EPooledActorType::ResourcePickup);
			}

			if (ensure(IsValid(_onCollectResourceSound)))
			{
				UGameplayStatics::SpawnSoundAtLocation(this, _onCollectResourceSound, GetActorLocation());
			}
		}
	}
	else
	{
		AddActorLocalRotation(FRotator(0.f, _rotationRate * DeltaTime * _rotationVariance, 0.f));
	}
}

void AResourcePickupActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AResourcePickupActor::AddActorToPool()
{
	_isMovingToActor = false;
	_isPlayerPickupPreventionTimeOver = false;
	_timeMovedToActor = 0;
	_actorToMoveTo = nullptr;
	_inventoryOfActorMovingTowards = nullptr;

	if (IsValid(_capsuleCollider))
	{
		_capsuleCollider->SetPhysicsLinearVelocity(FVector::ZeroVector);
		_capsuleCollider->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		_capsuleCollider->OnComponentHit.RemoveAll(this);
		_capsuleCollider->SetSimulatePhysics(false);
	}

	GetWorld()->GetTimerManager().ClearTimer(_pickupPreventionTimerHandle);

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	SetActorTickEnabled(false);
}

void AResourcePickupActor::RemoveFromPool()
{
	_isMovingToActor = false;
	_isPlayerPickupPreventionTimeOver = false;
	_isBeingThrownByPlayer = false;

	_capsuleCollider->SetSimulatePhysics(true);

	_capsuleCollider->SetPhysicsLinearVelocity(FVector::ZeroVector);
	_capsuleCollider->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	_startingHeight = GetActorLocation().Z;
	_rotationVariance = FMath::RandRange(.9f, 1.f);
	_bounceVariance = FMath::RandRange(.9f, 1.f);

	GetWorld()->GetTimerManager().SetTimer(_pickupPreventionTimerHandle, this, &AResourcePickupActor::OnPlayerPickupPreventionTimerEnd, _timeCannotMoveToPlayerAfterSpawn, false);

	if (ensure(IsValid(_capsuleCollider)))
	{
		_capsuleCollider->OnComponentHit.AddDynamic(this, &AResourcePickupActor::OnGroundHit);
	}

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &AResourcePickupActor::OnDayEnd);
	}

	SetActorTickEnabled(true);
}

bool AResourcePickupActor::AttemptMoveToActor(AActor* actor, UResourceInventory* actorInventory, const FVector& customEndLocation)
{
	if (!_isMovingToActor && ensure(IsValid(actor)) && ensure(IsValid(actorInventory)) && actorInventory->CanAddResource(_resourceType, _resourceAmount))
	{
		_actorToMoveTo = actor;
		_inventoryOfActorMovingTowards = actorInventory;
		_customEndLocation = customEndLocation;
		StartMovingTowardsActor();

		return true;
	}

	return false;
}

bool AResourcePickupActor::CanBeCollectedByPlayer() const
{
	return !_isMovingToActor && _isPlayerPickupPreventionTimeOver && !_isBeingThrownByPlayer;
}

void AResourcePickupActor::OnThrownOnGround()
{
	Cosmetic_OnHitGround();
}

void AResourcePickupActor::OnGroundHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ensure(IsValid(OtherActor)))
	{
		if (_isBeingThrownByPlayer)
		{
			OnThrownOnGround();
		}
		_isBeingThrownByPlayer = false;
	}
}

void AResourcePickupActor::OnPlayerPickupPreventionTimerEnd()
{
	_isPlayerPickupPreventionTimeOver = true;
	
	AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(FarmFPSUtilities::GetPlayerCharacter(this));
	if (ensure(IsValid(player)) && CanBeCollectedByPlayer() && player->IsPickupInRangeOfPlayer(this))
	{
		_actorToMoveTo = player;
		AttemptMoveToActor(_actorToMoveTo.Get(), player->FindComponentByClass<UResourceInventory>());
	}
}

void AResourcePickupActor::StartMovingTowardsActor()
{
	_startingMovementLocation = GetActorLocation();
	_isMovingToActor = true;
	_capsuleCollider->SetSimulatePhysics(false);
}

void AResourcePickupActor::OnDayEnd()
{
	UActorPool* actorPool = FarmFPSUtilities::GetActorPool(this);
	if (ensure(IsValid(actorPool)))
	{
		actorPool->AddActorToPool(_resourceType, this, EPooledActorType::ResourcePickup);
	}
}

void AResourcePickupActor::AddResourcesToInventory(UResourceInventory* inventory)
{
	if (ensure(IsValid(inventory)) && ensure(inventory->CanAddResource(_resourceType, _resourceAmount)))
	{
		inventory->AddResource(_resourceType, _resourceAmount);

		UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
		if (ensure(IsValid(objectiveManager)))
		{
			objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::CollectResource, _resourceType, _resourceAmount);
		}
	}
}
