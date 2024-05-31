// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Profile.generated.h"

/*
 * Enum representing different operating system platforms.
 */
UENUM(BlueprintType)
enum class EProfile : uint8
{
	NONE = 0 UMETA(DisplayName = "NONE"),
	BATTERY UMETA(DisplayName = "BATTERY"),
	CHARGING UMETA(DisplayName = "CHARGING"),
	DOCKED UMETA(DisplayName = "DOCKED"),
};
