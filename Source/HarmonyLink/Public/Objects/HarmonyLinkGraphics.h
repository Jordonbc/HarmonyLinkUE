// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Enums/Profile.h"
#include "Structs/SettingsProfile.h"

#include "UObject/Object.h"
#include "HarmonyLinkGraphics.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProfileChanged, EProfile, Profile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutomaticSwitchChanged, bool, bAutomaticSwich);

/**
 * 
 */
UCLASS(Blueprintable, config="HarmonyLink")
class HARMONYLINK_API UHarmonyLinkGraphics : public UObject
{
	GENERATED_BODY()

public:
	UHarmonyLinkGraphics();
	virtual ~UHarmonyLinkGraphics() override;

	UPROPERTY(BlueprintAssignable)
	FOnProfileChanged OnProfileChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAutomaticSwitchChanged OnAutomaticSwitchChanged;
	
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void LoadConfig(const bool bForceReload = false);

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SaveConfig() const;

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SetSetting(EProfile Profile, FName Setting, FHLConfigValue Value);

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	bool ApplyProfile(EProfile Profile);

	/** Returns the game local machine settings (resolution, windowing mode, scalability settings, etc...) */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	static UHarmonyLinkGraphics* GetSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink Settings")
	FSettingsProfile GetSettingProfile(const EProfile Profile);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink Settings")
	EProfile GetActiveProfile() const;

	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SetAutomaticSwitching(const bool bAutomaticSwitch);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink Settings")
	bool GetAutomaticSwitching() const;

	static void DestroySettings();

private:
	void Init();
	void Tick();
	void CreateDefaultConfigFile();
	bool LoadSettingsFromConfig();
	bool LoadSection(const FConfigFile& ConfigFile, const TPair<EProfile, FName> Profile);
	void SaveSection(FSettingsProfile& SettingsProfile, const bool bFlush = false) const;
	void LoadDefaults();

	void OnPostWorldInitialization(UWorld* World, UWorld::InitializationValues IVS);

	static void ResetInstance();

	static void ApplySetting(const TPair<FName, FHLConfigValue>& Setting);

	// Debugging
	void DebugPrintProfiles() const;
	static void PrintDebugSection(FSettingsProfile& SettingsProfile);

	static FString _IniLocation;

	uint8 _bAutomaticSwitch :1;

	// How many times to query HarmonyLinkLib for hardware info
	static int32 _TickRate;

	FTimerHandle _TickTimerHandle;

	EProfile _ActiveProfile = EProfile::NONE;

	TMap<EProfile, FName> _ProfileNames = {
		{EProfile::BATTERY, "Battery"},
		{EProfile::CHARGING, "Charging"},
		{EProfile::DOCKED, "Docked"},
	};

	TMap<EProfile, FSettingsProfile> _Profiles;

	static TMap<FName, TMap<FName, FHLConfigValue>> _DefaultSettings;

	static UHarmonyLinkGraphics* _INSTANCE;
};
