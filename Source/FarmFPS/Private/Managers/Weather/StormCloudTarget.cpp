// Fill out your copyright notice in the Description page of Project Settings.

#include "StormCloudTarget.h"

// Brock
#include "Managers/FarmFPSUtilities.h"
#include "Projectiles/CropResourceProjectile.h"
#include "WeatherManager.h"

// UE
#include "Kismet/GameplayStatics.h"

AStormCloudTarget::AStormCloudTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = _staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
}

void AStormCloudTarget::BeginPlay()
{
	Super::BeginPlay();
	
	if (ensure(IsValid(_staticMesh)))
	{
		_staticMesh->OnComponentBeginOverlap.RemoveAll(this);
		_staticMesh->OnComponentBeginOverlap.AddDynamic(this, &AStormCloudTarget::OnComponentOverlap);
	}

	if (IsValid(_onStormCloudSpawn))
	{
		UGameplayStatics::SpawnSoundAtLocation(this, _onStormCloudSpawn, GetActorLocation());
	}
}

void AStormCloudTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_direction.IsNearlyZero())
	{
		return;
	}

	SetActorLocation(GetActorLocation() + _direction * _speed * DeltaTime);
}

void AStormCloudTarget::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_staticMesh))
	{
		_staticMesh->OnComponentBeginOverlap.RemoveAll(this);
	}

	UWeatherManager* weatherManager = UFarmFPSUtilities::GetWeatherManager(this);
	if (IsValid(weatherManager) && !_wasShot)
	{
		weatherManager->StormCloudDisappeared();
	}

	Super::EndPlay(EndPlayReason);
}

void AStormCloudTarget::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACropResourceProjectile* cropResourceProjectile = Cast<ACropResourceProjectile>(OtherActor);
	if (IsValid(cropResourceProjectile))
	{
		UWeatherManager* weatherManager = UFarmFPSUtilities::GetWeatherManager(this);
		if (ensure(IsValid(weatherManager)))
		{
			weatherManager->StormCloudShot(cropResourceProjectile->GetProjectileType());
			Cosmetic_OnStormCloudHit(cropResourceProjectile->GetProjectileType());
			_wasShot = true;
		}
	}

	Destroy();
}

