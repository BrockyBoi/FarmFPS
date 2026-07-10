// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplodableResourcePickupActor.h"

// Brock
#include "Plants/Plant.h"

void AExplodableResourcePickupActor::ExplosionAffectActor(AActor* actor)
{
	Super::ExplosionAffectActor(actor);

	APlant* plant = Cast<APlant>(actor);
	if (IsValid(plant))
	{
		plant->AddResource(_explosionResourceType, _explosionResourceValue.GetModifiedValue(this));
	}
}
