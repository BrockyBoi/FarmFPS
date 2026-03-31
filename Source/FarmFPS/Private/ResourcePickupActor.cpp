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

	if (ensure(IsValid(_playerCollider)))
	{
		_playerCollider->OnComponentBeginOverlap.AddDynamic(this, &AResourcePickupActor::OnComponentOverlap);
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
	}

	if (IsValid(_capsuleCollider))
	{
		_capsuleCollider->OnComponentHit.RemoveAll(this);
	}

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

	AddActorLocalRotation(FRotator(0.f, _rotationRate * DeltaTime * _rotationVariance, 0.f));
}

void AResourcePickupActor::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFarmFPSCharacter* player = Cast<AFarmFPSCharacter>(OtherActor);
	if (IsValid(player))
	{
		UResourceInventory* inventory = player->FindComponentByClass<UResourceInventory>();
		if (IsValid(inventory) && inventory->CanAddResource(_cropType, _yieldAmount))
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
}

void AResourcePickupActor::OnCapsuleColliderHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ensure(IsValid(_capsuleCollider)))
	{
		//_capsuleCollider->SetSimulatePhysics(false);
	}
}

void AResourcePickupActor::OnDayEnd()
{
	Destroy();
}

