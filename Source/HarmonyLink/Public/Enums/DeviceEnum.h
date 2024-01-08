#pragma once

#include "CoreMinimal.h"
#include "DeviceEnum.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EDeviceEnum : uint8
{
    DESKTOP UMETA(DisplayName = "Desktop"),
    LAPTOP UMETA(DisplayName = "Laptop"),
    HANDHELD UMETA(DisplayName = "Handheld"),

    STEAM_DECK UMETA(DisplayName = "Steam Deck")
};
