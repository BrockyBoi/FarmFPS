// Fill out your copyright notice in the Description page of Project Settings.


#include "SeedProjectile.h"

// Sets default values
ASeedProjectile::ASeedProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASeedProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASeedProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ensure(IsValid(GetWorld())) && ensure(IsValid(_cropActorClass)))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(_cropActorClass, HitLocation, FRotator::ZeroRotator, SpawnParams);

		Destroy();
	}
}


