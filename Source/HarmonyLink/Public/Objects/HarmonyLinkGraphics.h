// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Structs/HLConfigValue.h"

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
	void LoadProfile(const FName& ProfileName, const bool bForceReload = false);

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SaveProfile(const FName& ProfileName);

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings", meta=(bCheckForCommandLineOverrides=true))
	void ApplySettings(bool bCheckForCommandLineOverrides = true);

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void ApplyNonResolutionSettings();

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void ApplyResolutionSettings(bool bCheckForCommandLineOverrides);

	/** Returns the game local machine settings (resolution, windowing mode, scalability settings, etc...) */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	static UHarmonyLinkGraphics* GetSettings();

	static void DestroySettings();

private:
	void CreateDefaultConfigFile();
	bool LoadSettingsFromConfig(const FString& SectionName);

	void SaveSection(const FName& SectionName, const TMap<FName, FHLConfigValue>& Settings) const;

	static void ResetInstance();

	UPROPERTY(Config)
	FName _ProfileName = NAME_None;

	static FString IniLocation;
	
	static FName BatteryProfile;
	static FName ChargingProfile;
	static FName DockedProfile;

	TMap<FName, int32> SettingsMap;

	// Maps to store configuration settings for each profile
	TMap<FName, FHLConfigValue> BatterySettings;
	TMap<FName, FHLConfigValue> ChargingSettings;
	TMap<FName, FHLConfigValue> DockedSettings;

	static TMap<FName, int32> DefaultSettingsMap;

	static UHarmonyLinkGraphics* Instance;
};
