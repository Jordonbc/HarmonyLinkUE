// Copyright (C) 2024 Jordon Brooks


#include "Objects/HarmonyLinkGraphics.h"
#include "ComponentRecreateRenderStateContext.h"
#include "HarmonyLink.h"

UHarmonyLinkGraphics* UHarmonyLinkGraphics::Instance = nullptr;
FString UHarmonyLinkGraphics::IniLocation = "HarmonyLink";
FName UHarmonyLinkGraphics::BatteryProfile = "Battery";
FName UHarmonyLinkGraphics::ChargingProfile = "Charging";
FName UHarmonyLinkGraphics::DockedProfile = "Docked";

TMap<FName, int32> UHarmonyLinkGraphics::DefaultSettingsMap = {{"Test", 0}};

UHarmonyLinkGraphics::UHarmonyLinkGraphics()
{
	UE_LOG(LogHarmonyLink, Warning, TEXT("HarmonyLinkGraphics initialized."));
	

	// Initialize settings for Low Graphics Profile
	BatterySettings.Add(TEXT("LowResolutionX"), FHLConfigValue(1280));
	BatterySettings.Add(TEXT("LowResolutionY"), FHLConfigValue(720));
	BatterySettings.Add(TEXT("LowTextureQuality"), FHLConfigValue(0.5f));

	// Initialize settings for Medium Graphics Profile
	ChargingSettings.Add(TEXT("MediumResolutionX"), FHLConfigValue(1920));
	ChargingSettings.Add(TEXT("MediumResolutionY"), FHLConfigValue(1080));
	ChargingSettings.Add(TEXT("MediumTextureQuality"), FHLConfigValue(0.75f));

	// Initialize settings for High Graphics Profile
	DockedSettings.Add(TEXT("HighResolutionX"), FHLConfigValue(3840));
	DockedSettings.Add(TEXT("HighResolutionY"), FHLConfigValue(2160));
	DockedSettings.Add(TEXT("HighTextureQuality"), FHLConfigValue(1.0f));
}

void UHarmonyLinkGraphics::LoadProfile(const FName& ProfileName, const bool bForceReload)
{
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_LoadSettings);

	// Construct the section name
	FString SectionName = FString::Printf(TEXT("GraphicsProfile.%s"), *ProfileName.ToString());

	// Clear the previous settings
	SettingsMap.Empty();

	// Load the settings into the map
	if (!LoadSettingsFromConfig(SectionName))
	{
		// Retry 2nd time
		LoadSettingsFromConfig(SectionName);
	}
}

bool UHarmonyLinkGraphics::LoadSettingsFromConfig(const FString& SectionName)
{
	// Load the configuration for the specified profile
	FConfigFile ConfigFile;

	// Normalize the INI file path
	const FString IniFilePath = FPaths::Combine(FPaths::ProjectConfigDir(), IniLocation + TEXT(".ini"));
	const FString NormalizedIniFilePath = FConfigCacheIni::NormalizeConfigIniPath(IniFilePath);

	if (FConfigCacheIni::LoadLocalIniFile(ConfigFile, *IniLocation, true, nullptr, false))
	{
		if (const FConfigSection* Section = ConfigFile.Find(*SectionName))
		{
			for (const auto& ValueIt : *Section)
			{
				int32 Value = FCString::Atoi(*ValueIt.Value.GetValue());
				SettingsMap.Add(*ValueIt.Key.ToString(), Value);
			}

			return true;
		}
	}

	CreateDefaultConfigFile();
	return false;
}

void UHarmonyLinkGraphics::SaveProfile(const FName& ProfileName)
{
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_SaveSettings);

	// Normalize the INI file path
	const FString IniFilePath = FPaths::Combine(FPaths::ProjectConfigDir(), IniLocation + TEXT(".ini"));
	const FString NormalizedIniFilePath = FConfigCacheIni::NormalizeConfigIniPath(IniFilePath);
	
	FConfigFile ConfigFile;
	ConfigFile.Write(NormalizedIniFilePath);
}

void UHarmonyLinkGraphics::ApplySettings(const bool bCheckForCommandLineOverrides)
{
	{
		FGlobalComponentRecreateRenderStateContext Context;
		ApplyResolutionSettings(bCheckForCommandLineOverrides);
		ApplyNonResolutionSettings();
	}

	SaveProfile(_ProfileName);
}

void UHarmonyLinkGraphics::ApplyNonResolutionSettings()
{
}

void UHarmonyLinkGraphics::ApplyResolutionSettings(bool bCheckForCommandLineOverrides)
{
}

UHarmonyLinkGraphics* UHarmonyLinkGraphics::GetSettings()
{
	if (!Instance)
	{
		Instance = NewObject<UHarmonyLinkGraphics>();
		Instance->AddToRoot();
		Instance->LoadProfile(ChargingProfile);
	}
	
	return Instance;
}

void UHarmonyLinkGraphics::DestroySettings()
{
	if (Instance)
	{
		Instance->RemoveFromRoot();
		Instance->MarkAsGarbage();
		Instance = nullptr;
	}
}

void UHarmonyLinkGraphics::CreateDefaultConfigFile()
{
	UE_LOG(LogHarmonyLink, Log, TEXT("CreateDefaultConfigFile started."));
	
	SaveSection(BatteryProfile, BatterySettings);
	SaveSection(ChargingProfile, ChargingSettings);
	SaveSection(DockedProfile, DockedSettings);
}

void UHarmonyLinkGraphics::SaveSection(const FName& SectionName, const TMap<FName, FHLConfigValue>& Settings) const
{
	if (GConfig)
	{
		const FString Filename = GetDefaultConfigFilename();
		for (const auto& Setting : Settings)
		{
			switch (Setting.Value.GetType())
			{
			case EConfigValueType::Int:
				GConfig->SetInt(*SectionName.ToString(), *Setting.Key.ToString(), Setting.Value.GetValue<int32>(), Filename);
				break;
			case EConfigValueType::Float:
				GConfig->SetFloat(*SectionName.ToString(), *Setting.Key.ToString(), Setting.Value.GetValue<float>(), Filename);
				break;
			case EConfigValueType::Bool:
				GConfig->SetBool(*SectionName.ToString(), *Setting.Key.ToString(), Setting.Value.GetValue<bool>(), Filename);
				break;
			case EConfigValueType::String:
				GConfig->SetString(*SectionName.ToString(), *Setting.Key.ToString(), *Setting.Value.GetValue<FString>(), Filename);
				break;
			}
		}

		UE_LOG(LogHarmonyLink, Log, TEXT("Saving config file: '%s'"), *Filename)
		GConfig->Flush(false, Filename);
	}
}

void UHarmonyLinkGraphics::ResetInstance()
{
	Instance->DestroySettings();
	GetSettings();
}
