// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Enums/Profile.h"
#include "Structs/SettingsProfile.h"

#include "UObject/Object.h"
#include "HarmonyLinkGraphics.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, config="HarmonyLink")
class HARMONYLINK_API UHarmonyLinkGraphics : public UObject
{
	GENERATED_BODY()

public:
	UHarmonyLinkGraphics();
	
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void LoadConfig(const bool bForceReload = false);

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SaveConfig() const;

	/** Returns the game local machine settings (resolution, windowing mode, scalability settings, etc...) */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	static UHarmonyLinkGraphics* GetSettings();

	static void DestroySettings();

private:
	void CreateDefaultConfigFile();
	bool LoadSettingsFromConfig();
	bool LoadSection(const FConfigFile& ConfigFile, const TPair<EProfile, FName> Profile);
	void SaveSection(FSettingsProfile& SettingsProfile, const bool bFlush = false) const;
	void LoadDefaults();
	bool ApplyProfile(EProfile Profile);

	static void ResetInstance();

	static void ApplySetting(const TPair<FName, FHLConfigValue>& Setting);

	// Debugging
	void DebugPrintProfiles() const;
	static void PrintDebugSection(FSettingsProfile& SettingsProfile);

	static FString IniLocation;

	TMap<EProfile, FName> ProfileNames = {
		{EProfile::BATTERY, "Battery"},
		{EProfile::CHARGING, "Charging"},
		{EProfile::DOCKED, "Docked"},
	};

	TMap<EProfile, FSettingsProfile> Profiles;

	static TMap<FName, TMap<FName, FHLConfigValue>> DefaultSettings;

	static UHarmonyLinkGraphics* Instance;
};
