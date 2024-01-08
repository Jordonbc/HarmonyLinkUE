// Copyright (C) 2023  Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Structs/Battery.h"
#include "Structs/CPUInfo.h"
#include "Structs/Device.h"
#include "Structs/OSVerInfo.h"
#include "HarmonyLinkLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HARMONYLINK_API UHarmonyLinkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static bool IsWine();
	
	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static bool IsLinux();

	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static bool IsSteamDeck();

	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FCPUInfo GetCPUInfo();

	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FDevice GetDeviceInfo();

	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FOSVerInfo GetOSInfo();

	UFUNCTION(BlueprintCallable, Category="HarmonyLink")
	static FBattery GetBatteryStatus();
};
