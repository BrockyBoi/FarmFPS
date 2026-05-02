// Fill out your copyright notice in the Description page of Project Settings.

#include "CropResourceProjectile.h"

// Brock
#include "CropComponent.h"
#include "MoonHitBox.h"
#include "ResourceTypeTags.h"

// Sets default values
ACropResourceProjectile::ACropResourceProjectile() : Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	_cropCollider = CreateDefaultSubobject<USphereComponent>(TEXT("CropCollider"));

	_cropCollider->SetupAttachment(RootComponent);
	_cropCollider->SetSphereRadius(16.0f);
	_cropCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_cropCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}

// Called when the game starts or when spawned
void ACropResourceProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_cropCollider)))
	{
		_cropCollider->OnComponentBeginOverlap.AddDynamic(this, &ACropResourceProjectile::OnComponentOverlap);
	}
}

void ACropResourceProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_cropCollider))
	{
		_cropCollider->OnComponentBeginOverlap.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ACropResourceProjectile::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ensure(IsValid(OtherActor)) && ensure(IsValid(GetOwner())))
	{
		UCropComponent* cropComponent = OtherActor->FindComponentByClass<UCropComponent>();
		if (IsValid(cropComponent))
		{
			cropComponent->AddCropResourceValue(_cropResourceType, _resourceAmount.GetModifiedValue(this));
		}

		UMoonHitBox* moonHitBox = OtherActor->FindComponentByClass<UMoonHitBox>();
		if (_cropResourceType == ResourceTypeTags::Light && IsValid(moonHitBox))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Hit moon with light resource! %s"), *GetActorLocation().ToString()));
			UE_LOG(LogTemp, Log, TEXT("Hit moon with light resource! %s"), *GetActorLocation().ToString());
			moonHitBox->HitMoon();
		}
	}
}

void ACropResourceProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}
