// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPerkModifierHandler.h"

// Brock
#include "PerkData.h"
#include "PerkManager.h"
#include "PerkModifierTypeTags.h"

// FarmFPS
#include "ShooterCharacter.h"

UPlayerPerkModifierHandler::UPlayerPerkModifierHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerPerkModifierHandler::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(_playerPickupCollider)))
	{
		_playerPickupCollider->SetSphereRadius(PlayerPickupColliderSizeModifier.GetBaseValue());
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

	if (perkTag == PerkModifierTypeTags::PlayerPickupColliderSize)
	{
		if (ensure(IsValid(_playerPickupCollider)))
		{
			_playerPickupCollider->SetSphereRadius(_perkManager->ModifyValueByPerks(perkTag, _playerPickupCollider->GetUnscaledSphereRadius()));
		}
	}
}
