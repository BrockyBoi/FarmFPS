// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourcePickupActor.h"

// Modules
#include "FarmFPSCharacter.h"

//Brock
#include "DayNightCycleManager.h"
#include "FarmFPSUtilities.h"
#include "ObjectiveManager.h"
#include "ObjectiveTypeTags.h"
#include "ResourceInventory.h"

// UE
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AResourcePickupActor::AResourcePickupActor()
{
	PrimaryActorTick.bCanEverTick = true;

	_capsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	SetRootComponent(_capsuleCollider);

	_staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	_staticMesh->SetupAttachment(RootComponent);

	_playerCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerCollider"));

	_playerCollider->SetupAttachment(RootComponent);
	_playerCollider->SetSphereRadius(16.0f);
	_playerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_playerCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	_playerCollider->SetGenerateOverlapEvents(true);
}

void AResourcePickupActor::BeginPlay()
{
	Super::BeginPlay();

	_startingHeight = GetActorLocation().Z;
	_rotationVariance = FMath::RandRange(.9f, 1.f);
	_bounceVariance = FMath::RandRange(.9f, 1.f);

	GetWorld()->GetTimerManager().SetTimer(_pickupPreventionTimerHandle, this, &AResourcePickupActor::OnPickupPreventionTimerEnd, _timeCannotMoveToPlayerAfterSpawn, false);

	if (ensure(IsValid(_playerCollider)))
	{
		_playerCollider->OnComponentBeginOverlap.AddDynamic(this, &AResourcePickupActor::OnComponentOverlap);
		_playerCollider->OnComponentEndOverlap.AddDynamic(this, &AResourcePickupActor::OnComponentOverlapEnd);
	}

	if (ensure(IsValid(_capsuleCollider)))
	{
		_capsuleCollider->OnComponentHit.AddDynamic(this, &AResourcePickupActor::OnCapsuleColliderHit);
	}

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayEnd.AddUObject(this, &AResourcePickupActor::OnDayEnd);
	}
}

void AResourcePickupActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_playerCollider))
	{
		_playerCollider->OnComponentBeginOverlap.RemoveAll(this);
		_playerCollider->OnComponentEndOverlap.RemoveAll(this);
	}

	if (IsValid(_capsuleCollider))
	{
		_capsuleCollider->OnComponentHit.RemoveAll(this);
	}

	GetWorld()->GetTimerManager().ClearTimer(_pickupPreventionTimerHandle);

	UDayNightCycleManager* dayNightCycle = FarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AResourcePickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_isMovingToPlayer && ensure(_characterToMoveTo.IsValid()))
	{
		SetActorLocation(FMath::Lerp(_startingMovementLocation, _characterToMoveTo->GetActorLocation(), _timeMovedToPlayer / _timeToMoveToPlayer));
		_timeMovedToPlayer += DeltaTime;

		if (_timeMovedToPlayer >= _timeToMoveToPlayer)
		{
			AddResourcesToPlayerInventory(_characterToMoveTo->FindComponentByClass<UResourceInventory>());
		}
	}
	else
	{
		AddActorLocalRotation(FRotator(0.f, _rotationRate * DeltaTime * _rotationVariance, 0.f));
	}
}

void AResourcePickupActor::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(OtherActor);
	if (IsValid(player))
	{
		UResourceInventory* inventory = player->FindComponentByClass<UResourceInventory>();
		if (IsValid(inventory) && inventory->CanAddResource(_cropType, _yieldAmount))
		{
			_characterToMoveTo = player;

			if (_isAllowedToBeCollected)
			{
				StartMovingTowardsPlayer();
			}
		}
	}
}

void AResourcePickupActor::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(OtherActor);
	if (IsValid(player))
	{
		if (!_isAllowedToBeCollected)
		{
			_characterToMoveTo = nullptr;
		}
	}
}

void AResourcePickupActor::OnCapsuleColliderHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ensure(IsValid(_capsuleCollider)))
	{
		//_capsuleCollider->SetSimulatePhysics(false);
	}
}

void AResourcePickupActor::OnPickupPreventionTimerEnd()
{
	_isAllowedToBeCollected = true;

	if (_characterToMoveTo.IsValid())
	{
		StartMovingTowardsPlayer();
	}
}

void AResourcePickupActor::StartMovingTowardsPlayer()
{
	_startingMovementLocation = GetActorLocation();
	_isMovingToPlayer = true;
	_capsuleCollider->SetSimulatePhysics(false);
}

void AResourcePickupActor::OnDayEnd()
{
	Destroy();
}

void AResourcePickupActor::AddResourcesToPlayerInventory(UResourceInventory* inventory)
{
	if (ensure(IsValid(inventory)) && ensure(inventory->CanAddResource(_cropType, _yieldAmount)))
	{
		inventory->AddResource(_cropType, _yieldAmount);

		UObjectiveManager* objectiveManager = FarmFPSUtilities::GetObjectiveManager(this);
		if (ensure(IsValid(objectiveManager)))
		{
			objectiveManager->IncrementObjectiveProgress(ObjectiveTypeTags::CollectResource, _cropType, _yieldAmount);
		}

		Destroy();
	}
}
