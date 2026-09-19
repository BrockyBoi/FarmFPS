// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/TutorialShineActor.h"

// UE
#include "Components/ArrowComponent.h"
#include "Components/SpotLightComponent.h"

ATutorialShineActor::ATutorialShineActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATutorialShineActor::BeginPlay()
{
	Super::BeginPlay();

	_spotLight = FindComponentByClass<USpotLightComponent>();
	_arrowComponent = FindComponentByClass<UArrowComponent>();
	
}

void ATutorialShineActor::Tick(float DeltaTime)
{
	if (ensure(_actorToFollow.IsValid()))
	{
		SetActorLocation(_actorToFollow->GetActorLocation() + (FVector::UpVector * _heightOverObject));
	}
}

void ATutorialShineActor::Show(const FVector& location, EShineSize shineSize)
{
	_actorToFollow = nullptr;

	SetActorHiddenInGame(false);

	if (ensure(IsValid(_spotLight)) && ensure(IsValid(_arrowComponent)))
	{
		float angle = 0;
		float arrowScale = 1;
		float heightScale = 1.f;
		switch (shineSize)
		{
		case EShineSize::Small:
			angle = _smallAngle;
			arrowScale = 1;
			heightScale = .9f;
			break;
		case EShineSize::Medium:
			angle = _mediumAngle;
			arrowScale = 1.5;
			heightScale = 1.15f;
			break;
		case EShineSize::Large:
			angle = _largeAngle;
			arrowScale = 2.f;
			heightScale = 1.25f;
		}
		_spotLight->SetOuterConeAngle(angle);
		_arrowComponent->SetRelativeScale3D(FVector::OneVector * arrowScale);
		SetActorLocation(location + (FVector::UpVector * _heightOverObject * heightScale));
	}
}

void ATutorialShineActor::FollowActor(AActor* actorToFollow, EShineSize shineSize)
{
	if (ensure(IsValid(actorToFollow)))
	{
		Show(actorToFollow->GetActorLocation(), shineSize);

		_actorToFollow = actorToFollow;
		SetActorTickEnabled(true);
	}
}

void ATutorialShineActor::Hide()
{
	SetActorHiddenInGame(true);
	_actorToFollow = nullptr;
	SetActorTickEnabled(false);
}