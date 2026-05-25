#pragma once

// UE
#include "CoreMinimal.h"

// Generated
#include "PooledActorTypes.generated.h"

UENUM()
enum class EPooledActorType : uint8
{
	Crop,
	Projectile,
	ResourcePickup
};