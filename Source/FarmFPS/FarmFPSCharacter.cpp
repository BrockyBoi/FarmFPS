// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmFPSCharacter.h"

// Brock
#include "ConstantCropAffectorArea.h"
#include "CropComponent.h"
#include "PerkManager.h"

// UE
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FarmFPS.h"

AFarmFPSCharacter::AFarmFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	_groundSlamSphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Ground Slam Sphere Collider"));
	_groundSlamSphereCollider->SetupAttachment(GetMesh());

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	_perkManager = CreateDefaultSubobject<UPerkManager>(TEXT("PerkManager"));

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AFarmFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_groundSlamSphereCollider)))
	{
		_groundSlamSphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFarmFPSCharacter::OnGroundSlamComponentOverlap);
		_groundSlamSphereCollider->SetSphereRadius(_groundSlamExplosionRadius.GetModifiedValue(this));
		_groundSlamSphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ensure(IsValid(_perkManager)))
	{
		_perkManager->OnPerkLevelChange.AddDynamic(this, &AFarmFPSCharacter::OnPerkLevelDataChanged);
		_startingJumpCount = JumpMaxCount;
		_startingJumpHeight = GetCharacterMovement()->JumpZVelocity;
	}
}

void AFarmFPSCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_groundSlamSphereCollider))
	{
		_groundSlamSphereCollider->OnComponentBeginOverlap.RemoveAll(this);
	}

	if (IsValid(_perkManager))
	{
		_perkManager->OnPerkLevelChange.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AFarmFPSCharacter::OnPerkLevelDataChanged(const FGameplayTag& perkType, const FPerkData& perkData)
{
	JumpMaxCount = _startingJumpCount + _extraJumpCount.GetModifiedValue(this);
	GetCharacterMovement()->JumpZVelocity = _startingJumpHeight + _extraJumpHeight.GetModifiedValue(this);
}

void AFarmFPSCharacter::OnGroundSlamComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		UCropComponent* cropComponent = OtherActor->FindComponentByClass<UCropComponent>();
		if (IsValid(cropComponent) && cropComponent->IsCropReadyToBreak())
		{
			cropComponent->BreakCrop();
		}
	}
}

void AFarmFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFarmFPSCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFarmFPSCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFarmFPSCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFarmFPSCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFarmFPSCharacter::LookInput);

		EnhancedInputComponent->BindAction(SpawnWaterAffectorAction, ETriggerEvent::Started, this, &AFarmFPSCharacter::OnPressSpawnWaterAffector);
		EnhancedInputComponent->BindAction(SpawnLightAffectorAction, ETriggerEvent::Started, this, &AFarmFPSCharacter::OnPressSpawnLightAffector);

		EnhancedInputComponent->BindAction(GroundSlamAction, ETriggerEvent::Started, this, &AFarmFPSCharacter::DoGroundSlamStart);
		EnhancedInputComponent->BindAction(GroundSlamAction, ETriggerEvent::Completed, this, &AFarmFPSCharacter::DoGroundSlamEnd);
	}
	else
	{
		UE_LOG(LogFarmFPS, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFarmFPSCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (GetCharacterMovement()->IsWalking() && PrevMovementMode == EMovementMode::MOVE_Falling)
	{
		_startedGroundSlam = false;
		GetCharacterMovement()->GravityScale = 1.f;
		_groundSlamSphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void AFarmFPSCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AFarmFPSCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AFarmFPSCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFarmFPSCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFarmFPSCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AFarmFPSCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AFarmFPSCharacter::OnPressSpawnWaterAffector()
{
	if (ensure(IsValid(_waterAffectorClass)))
	{
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<AConstantCropAffectorArea>(_waterAffectorClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	}
}

void AFarmFPSCharacter::OnPressSpawnLightAffector()
{
	if (ensure(IsValid(_lightAffectorClass)))
	{
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<AConstantCropAffectorArea>(_lightAffectorClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	}
}

void AFarmFPSCharacter::DoGroundSlamStart()
{
	if (_startedGroundSlam || GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->GravityScale = .1f;
}

void AFarmFPSCharacter::DoGroundSlamEnd()
{
	if (_startedGroundSlam || GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	if (APlayerController* controller = Cast<APlayerController>(GetController()))
	{
		FVector outLoc;
		FRotator outRot;
		controller->GetPlayerViewPoint(outLoc, outRot);
		FVector forwardVector = outRot.Vector();
		forwardVector.Normalize();

		FHitResult hitResult;

		bool hitGround = GetWorld()->LineTraceSingleByChannel(hitResult, GetActorLocation(), GetActorLocation() + (forwardVector * _groundSlamDistanceThreshold.GetModifiedValue(this)), ECollisionChannel::ECC_WorldStatic);
		if (hitGround)
		{
			GetCharacterMovement()->Launch(forwardVector * _groundSlamDashForce);
			_startedGroundSlam = true;

			GetCharacterMovement()->AirControl = .5f;
			//GetCharacterMovement()->GravityScale = 1.f;
			_groundSlamSphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}
