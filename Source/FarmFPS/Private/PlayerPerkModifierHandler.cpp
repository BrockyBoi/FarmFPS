// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPerkModifierHandler.h"

// Brock
#include "PerkData.h"
#include "PerkManager.h"
#include "PerkModifierTypeTags.h"

// FarmFPS
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacter.h"

UPlayerPerkModifierHandler::UPlayerPerkModifierHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerPerkModifierHandler::BeginPlay()
{
	Super::BeginPlay();

	_owningCharacter = Cast<AShooterCharacter>(GetOwner());
	if (ensure(IsValid(_owningCharacter)))
	{
		_owningCharacter->JumpMaxCount = JumpCountModifier.GetBaseValue();

		UCharacterMovementComponent* movementComponent = _owningCharacter->GetCharacterMovement();
		if (ensure(IsValid(movementComponent)))
		{
			movementComponent->JumpZVelocity = JumpHeightModifier.GetBaseValue();
		}

		_perkManager = _owningCharacter->FindComponentByClass<UPerkManager>();
		if (ensure(IsValid(_perkManager)))
		{
			_perkManager->OnPerkLevelChange.AddDynamic(this, &UPlayerPerkModifierHandler::OnPerkLevelChange);
		}

		if (ensure(IsValid(_playerPickupCollider)))
		{
			_playerPickupCollider->SetSphereRadius(PlayerPickupColliderSizeModifier.GetBaseValue());
		}
	}
}

void UPlayerPerkModifierHandler::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(_perkManager))
	{
		_perkManager->OnPerkLevelChange.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UPlayerPerkModifierHandler::OnPerkLevelChange(const FGameplayTag& perkTag, const FPerkData& perkData)
{
	if (!ensure(IsValid(_owningCharacter)) || !ensure(IsValid(_perkManager)))
	{
		return;
	}

	if (perkTag == PerkModifierTypeTags::JumpCount)
	{
		_owningCharacter->JumpMaxCount = FMath::RoundToInt(_perkManager->ModifyValueByPerks(perkTag, _owningCharacter->JumpMaxCount));
	}
	else if (perkTag == PerkModifierTypeTags::JumpHeight)
	{
		UCharacterMovementComponent* movementComponent = _owningCharacter->GetCharacterMovement();
		if (ensure(IsValid(movementComponent)))
		{
			movementComponent->JumpZVelocity = _perkManager->ModifyValueByPerks(perkTag, movementComponent->JumpZVelocity);
		}
	}
	else if (perkTag == PerkModifierTypeTags::PlayerPickupColliderSize)
	{
		if (ensure(IsValid(_playerPickupCollider)))
		{
			_playerPickupCollider->SetSphereRadius(_perkManager->ModifyValueByPerks(perkTag, _playerPickupCollider->GetUnscaledSphereRadius()));
		}
	}
}
