// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Enums/DeviceEnum.h"
#include "Enums/Platform.h"
#include <Structs/FDevice.h>

#include "Device.generated.h"

// Represents information about a specific device.
USTRUCT(BlueprintType)
struct FDevice
{
	GENERATED_BODY()

	FDevice() {}

	// The platform on which the device operates. Note: This can differ from the build platform.
	// For example, if the device is identified as running on Wine, this will show Linux,
	// regardless of the build being an executable for Windows.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	EPlatform Platform = EPlatform::WINDOWS;

	// The type of the device.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	EDeviceEnum Device = EDeviceEnum::DESKTOP;

	// Constructor that initializes the struct with information from an external source.
	// @param oldDevice Pointer to an external FDevice structure to copy data from.
	FDevice(HarmonyLinkLib::FDevice* oldDevice);

private:
	// Converts an external device enum to the internal EDeviceEnum type.
	// @param Device External device enum to convert.
	// @returns Converted EDeviceEnum value.
	static EDeviceEnum Convert(HarmonyLinkLib::EDevice Device);
	
	// Converts an external platform enum to the internal EPlatform type.
	// @param Platform External platform enum to convert.
	// @returns Converted EPlatform value.
	static EPlatform Convert(HarmonyLinkLib::EPlatform Platform);
};
