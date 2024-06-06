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

public:
	UHarmonyLinkLibrary();

	// Checks if the game is running under Wine.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static bool IsWine(bool bForce = false);

	// Checks if the operating system is Linux.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static bool IsLinux(bool bForce = false);

	// Checks if the game is running on a Steam Deck.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static bool IsSteamDeck(bool bForce = false);

	// Retrieves information about the CPU of the current device.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static FCPUInfo GetCPUInfo(bool bForce = false);

	// Retrieves information about the current device.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static FDevice GetDeviceInfo(bool bForce = false);

	// Retrieves information about the operating system of the current device.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static FOSVerInfo GetOSInfo(bool bForce = false);

	// Retrieves the current battery status of the device.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink")
	static FBattery GetBatteryStatus();

private:
	static bool bIsWineCached;
	static bool bIsWine;
	static bool bIsLinuxCached;
	static bool bIsLinux;
	static bool bIsSteamDeckCached;
	static bool bIsSteamDeck;

	static bool bCPUInfoCached;
	static bool bDeviceInfoCached;
	static bool bOSInfoCached;

	static FCPUInfo CachedCPUInfo;
	static FDevice CachedDeviceInfo;
	static FOSVerInfo CachedOSInfo;
};
