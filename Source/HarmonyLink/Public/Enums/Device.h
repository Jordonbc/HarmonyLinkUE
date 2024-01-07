#pragma once

#include "CoreMinimal.h"
#include "Device.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EDevice : uint8
{
    DESKTOP UMETA(DisplayName = "Desktop"),
    LAPTOP UMETA(DisplayName = "Laptop"),
    HANDHELD UMETA(DisplayName = "Handheld"),

    STEAM_DECK UMETA(DisplayName = "Steam Deck")
};
