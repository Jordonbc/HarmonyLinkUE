// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"

#include "Structs/Battery.h"
#include "Structs/CPUInfo.h"
#include "Structs/Device.h"
#include "Structs/OSVerInfo.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HarmonyLinkLibrary.generated.h"

/**
 * Library of static functions for accessing various system information, particularly for the HarmonyLink project.
 */
UCLASS()
class HARMONYLINK_API UHarmonyLinkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Checks if the game is running under Wine.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static bool IsWine();

	// Checks if the operating system is Linux.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static bool IsLinux();

	// Checks if the game is running on a Steam Deck.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static bool IsSteamDeck();

	// Retrieves information about the CPU of the current device.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FCPUInfo GetCPUInfo();

	// Retrieves information about the current device.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FDevice GetDeviceInfo();

	// Retrieves information about the operating system of the current device.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FOSVerInfo GetOSInfo();

	// Retrieves the current battery status of the device.
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FBattery GetBatteryStatus();
};
