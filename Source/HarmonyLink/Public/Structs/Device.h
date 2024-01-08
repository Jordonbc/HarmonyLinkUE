#pragma once

#include "CoreMinimal.h"
#include "Enums/DeviceEnum.h"
#include "Enums/Platform.h"
#include <Structs/FDevice.h>

#include "Device.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FDevice
{
	GENERATED_BODY()

	FDevice() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlatform Platform = EPlatform::WINDOWS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDeviceEnum Device = EDeviceEnum::DESKTOP;

	FDevice(HarmonyLinkLib::FDevice* oldDevice);

	static EDeviceEnum Convert(HarmonyLinkLib::EDevice Device);
	static EPlatform Convert(HarmonyLinkLib::EPlatform Platform);
};
