// Copyright (C) 2023  Jordon Brooks
#pragma once

#include <HarmonyLinkLib.h>

#include "CoreMinimal.h"
#include "Battery.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FBattery
{
	GENERATED_BODY()

	FBattery() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool HasBattery = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsACConnected = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BatteryPercent = 0;

	FBattery(HarmonyLinkLib::FBattery* battery);
};
