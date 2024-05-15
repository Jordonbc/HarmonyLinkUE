// Copyright (C) 2024 Jordon Brooks


#include "Objects/HarmonyLinkGraphics.h"
#include "ComponentRecreateRenderStateContext.h"
#include "HarmonyLink.h"

UHarmonyLinkGraphics* UHarmonyLinkGraphics::Instance = nullptr;
FString UHarmonyLinkGraphics::IniLocation = "HarmonyLink";

TMap<FName, TMap<FName, FHLConfigValue>> UHarmonyLinkGraphics::DefaultSettings = {
	{ "Battery", {
		            { TEXT("r.ReflectionMethod"), FHLConfigValue(0) },
					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(0) }
	}},
	{ "Charging", {
		            { TEXT("r.ReflectionMethod"), FHLConfigValue(2) },
					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(1) }
	}},
	{ "Docked", {
		            { TEXT("r.ReflectionMethod"), FHLConfigValue(2) },
					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(1) }
	}}
};

UHarmonyLinkGraphics::UHarmonyLinkGraphics()
{
	UE_LOG(LogHarmonyLink, Warning, TEXT("HarmonyLinkGraphics initialized."));
}

void UHarmonyLinkGraphics::LoadConfig(const bool bForceReload)
{
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_LoadSettings);

	// Load the settings into the map
	if (!LoadSettingsFromConfig())
	{
		// Retry 2nd time
		LoadSettingsFromConfig();
	}

	DebugPrintProfiles();
}

bool UHarmonyLinkGraphics::LoadSettingsFromConfig()
{
	// Load the configuration from the INI file
	FConfigFile ConfigFile;

	const FString Filename = GetDefaultConfigFilename();

	if (FConfigCacheIni::LoadLocalIniFile(ConfigFile, *Filename, true, nullptr, false))
	{
		// Load each profile section
		bool bLoaded = true;

		for (const TPair<EProfile, FName>& Profile : ProfileNames)
		{
			if (!LoadSection(ConfigFile, Profile))
			{
				UE_LOG(LogHarmonyLink, Error, TEXT("Failed to load section: '%s'"), *Profile.Value.ToString());
				bLoaded = false;
			}
		}

		// Check if all profiles were loaded successfully
		if (bLoaded)
		{
			UE_LOG(LogHarmonyLink, Log, TEXT("Successfully loaded config file: %s"), *Filename);
			return true;
		}
	}

	UE_LOG(LogHarmonyLink, Error, TEXT("Failed to load config file: %s"), *Filename);
	CreateDefaultConfigFile();
	return false;
}

bool UHarmonyLinkGraphics::LoadSection(const FConfigFile& ConfigFile, const TPair<EProfile, FName> Profile)
{
	const FName& SectionName = Profile.Value;
	const EProfile ProfileKey = Profile.Key;

	if (const FConfigSection* Section = ConfigFile.FindSection(*SectionName.ToString()))
	{
		FSettingsProfile& SettingsProfile = Profiles.FindOrAdd(ProfileKey);
		SettingsProfile.SectionName = SectionName;
		SettingsProfile.Settings.Empty();  // Clear previous settings

		for (const auto& ValueIt : *Section)
		{
			FString ValueString = ValueIt.Value.GetValue();
			FString Value, Type;

			// Parse the Value and Type from the string
			if (ValueString.Split(TEXT(", Type="), &Value, &Type))
			{
				Value = Value.RightChop(7); // Remove "(Value=" prefix
				Type = Type.LeftChop(1); // Remove ")" suffix

				if (Type == "int")
				{
					const int32 IntValue = FCString::Atoi(*Value);
					SettingsProfile.Settings.Add(ValueIt.Key, FHLConfigValue(IntValue));
				}
				else if (Type == "float")
				{
					const float FloatValue = FCString::Atof(*Value);
					SettingsProfile.Settings.Add(ValueIt.Key, FHLConfigValue(FloatValue));
				}
				else if (Type == "bool")
				{
					const bool BoolValue = Value == "true";
					SettingsProfile.Settings.Add(ValueIt.Key, FHLConfigValue(BoolValue));
				}
				else if (Type == "string")
				{
					SettingsProfile.Settings.Add(ValueIt.Key, FHLConfigValue(Value));
				}
			}
		}

		return true;
	}

	return false;
}

void UHarmonyLinkGraphics::SaveConfig() const
{
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_SaveConfig);

	for (TPair<EProfile, FSettingsProfile> Profile : Profiles)
	{
		SaveSection(Profile.Value);
	}

	const FString Filename = GetDefaultConfigFilename();
	UE_LOG(LogHarmonyLink, Log, TEXT("Flushing file: '%s'"), *Filename);
	GConfig->Flush(false, Filename);
}

void UHarmonyLinkGraphics::ApplySettings(const bool bCheckForCommandLineOverrides)
{
	{
		FGlobalComponentRecreateRenderStateContext Context;
		//ApplyResolutionSettings(bCheckForCommandLineOverrides);
		//ApplyNonResolutionSettings();
	}
}



UHarmonyLinkGraphics* UHarmonyLinkGraphics::GetSettings()
{
	if (!Instance)
	{
		Instance = NewObject<UHarmonyLinkGraphics>();
		Instance->AddToRoot();
		Instance->LoadConfig();
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
	UE_LOG(LogHarmonyLink, Log, TEXT("Creating default config file."));

	LoadDefaults();
	SaveConfig();
	
	UE_LOG(LogHarmonyLink, Log, TEXT("Default config file created."));
}

void UHarmonyLinkGraphics::SaveSection(FSettingsProfile& SettingsProfile, const bool bFlush) const
{
	if (GConfig)
	{
		const FString Filename = GetDefaultConfigFilename();
		for (const auto& Setting : SettingsProfile.Settings)
		{
			FString TypeString;
			FString ValueString;

			switch (Setting.Value.GetType())
			{
			case EConfigValueType::Int:
				ValueString = FString::FromInt(Setting.Value.GetValue<int32>());
				TypeString = TEXT("int");
				break;
			case EConfigValueType::Float:
				ValueString = FString::SanitizeFloat(Setting.Value.GetValue<float>());
				TypeString = TEXT("float");
				break;
			case EConfigValueType::Bool:
				ValueString = Setting.Value.GetValue<bool>() ? TEXT("true") : TEXT("false");
				TypeString = TEXT("bool");
				break;
			case EConfigValueType::String:
				ValueString = Setting.Value.GetValue<FString>();
				TypeString = TEXT("string");
				break;
			}

			const FString ConfigValue = FString::Printf(TEXT("(Value=%s, Type=%s)"), *ValueString, *TypeString);
			GConfig->SetString(*SettingsProfile.SectionName.ToString(), *Setting.Key.ToString(), *ConfigValue, Filename);
		}

		UE_LOG(LogHarmonyLink, Log, TEXT("Saving config file: '%s'"), *Filename);
		if (bFlush)
		{
			UE_LOG(LogHarmonyLink, Log, TEXT("Flushing file: '%s'"), *Filename);
			GConfig->Flush(false, Filename);
		}
	}
}

void UHarmonyLinkGraphics::LoadDefaults()
{
	UE_LOG(LogHarmonyLink, Log, TEXT("LoadDefaults started."));

	Profiles.Reset();

	// Iterate over ProfileNames to create default settings
	for (const TPair<EProfile, FName>& Profile : ProfileNames)
	{
		FSettingsProfile NewProfileSettings;
		NewProfileSettings.SectionName = Profile.Value;

		if (const TMap<FName, FHLConfigValue>* Settings = DefaultSettings.Find(Profile.Value))
		{
			NewProfileSettings.Settings = *Settings;
		}

		Profiles.Add(Profile.Key, NewProfileSettings);
	}
}

void UHarmonyLinkGraphics::DebugPrintProfiles() const
{
	UE_LOG(LogHarmonyLink, Log, TEXT("DebugPrintProfiles started."));

	for (TPair<EProfile, FSettingsProfile> Profile : Profiles)
	{
		PrintDebugSection(Profile.Value);
	}
    
	UE_LOG(LogHarmonyLink, Log, TEXT("DebugPrintProfiles completed."));
}

void UHarmonyLinkGraphics::PrintDebugSection(FSettingsProfile& SettingsProfile)
{
	UE_LOG(LogHarmonyLink, Warning, TEXT("[%s]"), *SettingsProfile.SectionName.ToString());

	for (const auto& Setting : SettingsProfile.Settings)
	{
		FString ValueString;
		FString TypeString;

		switch (Setting.Value.GetType())
		{
		case EConfigValueType::Int:
			ValueString = FString::FromInt(Setting.Value.GetValue<int32>());
			TypeString = TEXT("int");
			break;
		case EConfigValueType::Float:
			ValueString = FString::SanitizeFloat(Setting.Value.GetValue<float>());
			TypeString = TEXT("float");
			break;
		case EConfigValueType::Bool:
			ValueString = Setting.Value.GetValue<bool>() ? TEXT("true") : TEXT("false");
			TypeString = TEXT("bool");
			break;
		case EConfigValueType::String:
			ValueString = Setting.Value.GetValue<FString>();
			TypeString = TEXT("string");
			break;
		}

		UE_LOG(LogHarmonyLink, Warning, TEXT("Key: %s = V=%s, T=%s "), *Setting.Key.ToString(), *ValueString, *TypeString);
	}
}

void UHarmonyLinkGraphics::ResetInstance()
{
	Instance->DestroySettings();
	GetSettings();
}
