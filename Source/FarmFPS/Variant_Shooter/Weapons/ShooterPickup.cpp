// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterPickup.h"

// Brock
#include "Managers/FarmFPSUtilities.h"
#include "SaveSystem/FarmFPSSaveGame.h"
#include "SaveSystem/WeaponPickupSaveGameData.h"
#include "SaveSystem/SaveGameManager.h"

// Variant_Shooter
#include "ShooterWeaponHolder.h"
#include "ShooterWeapon.h"

// UE
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

AShooterPickup::AShooterPickup()
{
 	PrimaryActorTick.bCanEverTick = true;

	// create the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// create the collision sphere
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(RootComponent);

	SphereCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 84.0f));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECC_WorldStatic);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->bFillCollisionUnderneathForNavmesh = true;

	// subscribe to the collision overlap on the sphere
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AShooterPickup::OnOverlap);

	// create the mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);

	Mesh->SetCollisionProfileName(FName("NoCollision"));
}

void AShooterPickup::BeginPlay()
{
	Super::BeginPlay();

	if (FWeaponTableRow* WeaponData = WeaponType.GetRow<FWeaponTableRow>(FString()))
	{
		// copy the weapon class
		WeaponClass = WeaponData->WeaponToSpawn;
	}

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (ensure(IsValid(saveGameManager)))
	{
		saveGameManager->OnLoadGameData.AddUObject(this, &AShooterPickup::OnGameLoaded);
	}
}

void AShooterPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// clear the respawn timer
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);

	USaveGameManager* saveGameManager = UFarmFPSUtilities::GetSaveGameManager(this);
	if (IsValid(saveGameManager))
	{
		saveGameManager->OnLoadGameData.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

FWeaponPickupSaveGameData AShooterPickup::GetWeaponPickupSaveGameData() const
{
	FWeaponPickupSaveGameData saveData;
	saveData.HasBeenPickedUp = HasBeenPickedUp();
	saveData.WeaponTypeTag = GetWeaponTypeTag();

	return saveData;
}

FGameplayTag AShooterPickup::GetWeaponTypeTag() const
{
	if (FWeaponTableRow* WeaponData = WeaponType.GetRow<FWeaponTableRow>(FString()))
	{
		return WeaponData->WeaponTypeTag;
	}

	return FGameplayTag::EmptyTag;
}

void AShooterPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (FWeaponTableRow* WeaponData = WeaponType.GetRow<FWeaponTableRow>(FString()))
	{
		Mesh->SetStaticMesh(WeaponData->StaticMesh.LoadSynchronous());
	}
}

void AShooterPickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// have we collided against a weapon holder?
	if (IShooterWeaponHolder* WeaponHolder = Cast<IShooterWeaponHolder>(OtherActor))
	{
		WeaponHolder->AddWeaponClass(WeaponClass);

		// hide this mesh
		SetActorHiddenInGame(true);

		// disable collision
		SetActorEnableCollision(false);

		// disable ticking
		SetActorTickEnabled(false);

		// schedule the respawn
		//GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AShooterPickup::RespawnPickup, RespawnTime, false);
		_hasBeenPickedUp = true;
	}
}

void AShooterPickup::RespawnPickup()
{
	// unhide this pickup
	SetActorHiddenInGame(false);

	// call the BP handler
	BP_OnRespawn();
}

void AShooterPickup::FinishRespawn()
{
	// enable collision
	SetActorEnableCollision(true);

	// enable tick
	SetActorTickEnabled(true);
}

void AShooterPickup::OnGameLoaded(UFarmFPSSaveGame* saveGame)
{
	if (ensure(IsValid(saveGame)))
	{
		TArray<FWeaponPickupSaveGameData> weaponPickupSaveDatas = saveGame->GetWeaponPickupSaveDatas();

		FWeaponPickupSaveGameData* saveData = weaponPickupSaveDatas.FindByPredicate([&](const FWeaponPickupSaveGameData& saveData)
		{
			return saveData.WeaponTypeTag == GetWeaponTypeTag();
		});

		if (saveData)
		{
			_hasBeenPickedUp = saveData->HasBeenPickedUp;
			if (_hasBeenPickedUp)
			{
				SetActorHiddenInGame(true);
				SetActorEnableCollision(false);
				SetActorTickEnabled(false);
			}
		}
	}
}
