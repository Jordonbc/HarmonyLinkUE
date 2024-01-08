// Copyright (C) 2024 Jordon Brooks
#pragma once

#include "CoreMinimal.h"
#include <HarmonyLinkLib.h>

#include "OSVerInfo.generated.h"

// Represents information about an operating system version.
USTRUCT(BlueprintType)
struct FOSVerInfo
{
	GENERATED_BODY()

	FOSVerInfo() {}

	// The name of the operating system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString Name;

	// Numerical version of the operating system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	int32 Version = 0;

	// Unique identifier for the operating system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString ID;

	// Identifier for the specific version of the operating system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString VersionID;

	// Codename for the operating system version.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString VersionCodename;

	// User-friendly name for the operating system version.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString PrettyName;

	// Variant identifier of the operating system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HarmonyLink")
	FString VariantID;

	// Constructor that initializes the struct with information from an external source.
	// @param oldInfo Pointer to an external FOSVerInfo structure to copy data from.
	FOSVerInfo(HarmonyLinkLib::FOSVerInfo* oldInfo);
};
