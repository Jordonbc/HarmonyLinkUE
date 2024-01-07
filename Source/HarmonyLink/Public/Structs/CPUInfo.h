// Copyright (C) 2023  Jordon Brooks

#pragma once

#include <HarmonyLinkLib.h>

#include "CoreMinimal.h"
#include "CPUInfo.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FCPUInfo
{
	GENERATED_BODY()
	
	FCPUInfo() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString VendorID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ModelName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PhysicalCores = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 LogicalCores = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSet<FString> Flags;

	FCPUInfo(HarmonyLinkLib::FCPUInfo* cpu_info);
};
