// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourcePickupActor.h"

//Brock
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

	Super::EndPlay(EndPlayReason);
}

void AResourcePickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, _startingHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * _bounceRate * _bounceVariance) * 10.f));
	AddActorLocalRotation(FRotator(0.f, _rotationRate * DeltaTime * _rotationVariance, 0.f));
}

void AResourcePickupActor::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		UResourceInventory* inventory = OtherActor->FindComponentByClass<UResourceInventory>();
		if (IsValid(inventory) && inventory->CanAddResource(_cropType, _yieldAmount))
		{
			inventory->AddResource(_cropType, _yieldAmount);

			Destroy();
		}
	}
}

void AResourcePickupActor::OnCapsuleColliderHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ensure(IsValid(_capsuleCollider)))
	{
		_capsuleCollider->SetSimulatePhysics(false);
	}
}

