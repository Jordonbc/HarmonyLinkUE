// Copyright (C) 2024 Jordon Brooks
#pragma once

#include <HarmonyLinkLib.h>

#include "CoreMinimal.h"
#include "Battery.generated.h"

/*
 * Represents the battery status and information of a device.
 */
USTRUCT(BlueprintType)
struct FBattery
{
	GENERATED_BODY()

	FBattery() {}

	// Indicates whether the device has a battery.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	bool HasBattery = false;

	// Indicates whether the device is connected to AC power.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	bool IsACConnected = false;

	// The current battery percentage of the device.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	int32 BatteryPercent = 0;

	// Constructor that initializes the struct with information from an external battery source.
	// @param battery Pointer to an external FBattery structure to copy data from.
	FBattery(HarmonyLinkLib::FBattery* battery);
};
