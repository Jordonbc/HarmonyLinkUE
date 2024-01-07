#pragma once

#include "CoreMinimal.h"
#include "Enums/Device.h"
#include "Enums/Platform.h"

#include "FDevice.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FDevice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlatform Platform = EPlatform::WINDOWS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDevice Device = EDevice::DESKTOP;
};
