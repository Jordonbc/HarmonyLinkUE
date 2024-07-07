// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "HLConfigValue.h"

#include "SettingsProfile.generated.h"

USTRUCT(BlueprintType)
struct FSettingsProfile
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FName SectionName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	TMap<FName, FHLConfigValue> Settings;

	// Equality operators
	bool operator==(const FSettingsProfile& Other) const
	{
		return SectionName == Other.SectionName && Settings.OrderIndependentCompareEqual(Other.Settings);
	}

	bool operator!=(const FSettingsProfile& Other) const
	{
		return !(*this == Other);
	}
};

// Hash function
FORCEINLINE uint32 GetTypeHash(const FSettingsProfile& Profile)
{
	uint32 Hash = GetTypeHash(Profile.SectionName);

	for (const auto& Pair : Profile.Settings)
	{
		Hash = HashCombine(Hash, GetTypeHash(Pair.Key));
		Hash = HashCombine(Hash, GetTypeHash(Pair.Value));
	}

	return Hash;
}
