// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Platform.generated.h"

/*
 * Enum representing different operating system platforms.
 */
UENUM(BlueprintType)
enum class EPlatform : uint8
{
    WINDOWS UMETA(DisplayName = "Windows"),
    LINUX UMETA(DisplayName = "Linux"),
    MAC UMETA(DisplayName = "Mac"),
    UNIX UMETA(DisplayName = "Unix")
};
