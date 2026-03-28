// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstantCropAffectorArea.h"

// Brock
#include "CropComponent.h"

AConstantCropAffectorArea::AConstantCropAffectorArea()
{
	PrimaryActorTick.bCanEverTick = true;

	_cropAffectorArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CropAffectorArea"));
}

void AConstantCropAffectorArea::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_cropAffectorArea)))
	{
		_cropAffectorArea->OnComponentBeginOverlap.AddDynamic(this, &AConstantCropAffectorArea::OnComponentOverlap);
		_cropAffectorArea->SetCapsuleSize(_affectorRadius.GetModifiedValue(this), _affectorHeight.GetModifiedValue(this));
	}

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AConstantCropAffectorArea::OnLifeTimeEnd, _lifetime.GetModifiedValue(this), false);
}

void AConstantCropAffectorArea::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_cropAffectorArea))
	{
		_cropAffectorArea->OnComponentBeginOverlap.RemoveAll(this);
	}

	for (TWeakObjectPtr<UCropComponent> crop : _cropsInArea)
	{
		if (crop.IsValid())
		{
			crop->OnCropBreak.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AConstantCropAffectorArea::OnLifeTimeEnd()
{
	Destroy();
}

void AConstantCropAffectorArea::OnCropDestroyed(UCropComponent* cropDestroyed)
{
	if (IsValid(cropDestroyed))
	{
		_cropsInArea.Remove(cropDestroyed);
		cropDestroyed->OnCropBreak.RemoveAll(this);
	}
}

void AConstantCropAffectorArea::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		UCropComponent* cropComponent = OtherActor->FindComponentByClass<UCropComponent>();
		if (IsValid(cropComponent))
		{
			_cropsInArea.AddUnique(cropComponent);

			cropComponent->OnCropBreak.AddLambda([&, cropComponent]()
			{
				OnCropDestroyed(cropComponent);
			});
		}
	}
}

void AConstantCropAffectorArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TWeakObjectPtr<UCropComponent> crop : _cropsInArea)
	{
		if (crop.IsValid())
		{
			crop->AddCropResourceValue(_cropResourceType, _resourceAmountPerSecond.GetModifiedValue(this) * DeltaTime);
		}
	}
}

