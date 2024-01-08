// Copyright (C) 2024 Jordon Brooks

#pragma once

#include <HarmonyLinkLib.h>

#include "CoreMinimal.h"
#include "CPUInfo.generated.h"

/*
 * Represents information about the CPU of a device.
 */
USTRUCT(BlueprintType)
struct FCPUInfo
{
	GENERATED_BODY()
	
	FCPUInfo() {}

	// The vendor identifier for the CPU.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString VendorID;

	// The model name of the CPU.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString ModelName;

	// The number of physical cores in the CPU.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	int32 PhysicalCores = 0;

	// The number of logical cores in the CPU (may be different from physical cores).
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	int32 LogicalCores = 0;

	// A set of flags representing various features or capabilities of the CPU.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	TSet<FString> Flags;

	// Constructor that initializes the struct with information from an external CPU info source.
	// @param cpu_info Pointer to an external FCPUInfo structure to copy data from.
	FCPUInfo(HarmonyLinkLib::FCPUInfo* cpu_info);
};
