// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "DeviceEnum.generated.h"

/*
 * Enum representing different operating system platforms.
 */
UENUM(BlueprintType)
enum class EDeviceEnum : uint8
{
    DESKTOP UMETA(DisplayName = "Desktop"),
    LAPTOP UMETA(DisplayName = "Laptop"),
    HANDHELD UMETA(DisplayName = "Handheld"),

    STEAM_DECK UMETA(DisplayName = "Steam Deck")
};
