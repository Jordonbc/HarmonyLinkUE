// Copyright (C) 2024 Jordon Brooks


#include "Objects/HarmonyLinkGraphics.h"
#include "ComponentRecreateRenderStateContext.h"
#include "HarmonyLink.h"
#include "HarmonyLinkLibrary.h"
#include "GameFramework/GameUserSettings.h"

UHarmonyLinkGraphics* UHarmonyLinkGraphics::_INSTANCE = nullptr;
int32 UHarmonyLinkGraphics::_TickRate = 1;
FTimerHandle UHarmonyLinkGraphics::_TickTimerHandle = FTimerHandle();

/**
 * @brief Default graphics settings for different power states in HarmonyLink.
 * 
 * This map defines the default configuration values for various graphics settings based on the power state of the device.
 * The outer map's key is the power state (e.g., "Battery", "Charging", "Docked").
 * The inner map contains key-value pairs where the key is the setting name and the value is the corresponding configuration value.
 * 
 * Power States:
 * - "Battery": Settings for when the device is running on battery power.
 * - "Charging": Settings for when the device is charging.
 * - "Docked": Settings for when the device is docked.
 */
TMap<FName, TMap<FName, FHLConfigValue>> UHarmonyLinkGraphics::_DefaultSettings = {
	{ "Battery", {
		            { TEXT("r.ReflectionMethod"), FHLConfigValue(0) },
					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(0) },
					{ TEXT("r.ScreenPercentage"), FHLConfigValue(50) },
	}},
	{ "Charging", {
		            { TEXT("r.ReflectionMethod"), FHLConfigValue(2) },
					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(1) },
					{ TEXT("r.ScreenPercentage"), FHLConfigValue(100) },
	}},
	{ "Docked",	{
		            { TEXT("r.ReflectionMethod"), FHLConfigValue(2) },
					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(1) },
					{ TEXT("r.ScreenPercentage"), FHLConfigValue(100) },
	}}
};

UHarmonyLinkGraphics::UHarmonyLinkGraphics()
{
	UE_LOG(LogHarmonyLink, Warning, TEXT("HarmonyLinkGraphics initialized."));
	
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UHarmonyLinkGraphics::OnPostWorldInitialization);
	FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &UHarmonyLinkGraphics::OnWorldEnd);
}

UHarmonyLinkGraphics::~UHarmonyLinkGraphics()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
}

void UHarmonyLinkGraphics::LoadConfig(const bool bForceReload)
{
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_LoadSettings);

	// Load the settings into the map
	if (!LoadSettingsFromConfig(false))
	{
		if (!LoadSettingsFromConfig(true))
		{
			CreateDefaultConfigFile();
			// Retry 2nd time
			LoadSettingsFromConfig(true);
		}
	}

	DebugPrintProfiles();
}

bool UHarmonyLinkGraphics::LoadSettingsFromConfig(const bool bLoadDefaults)
{
	// Load the configuration from the INI file
	FConfigFile ConfigFile;

	const FString Filename = GetConfigDirectoryFile(bLoadDefaults);

	if (!GConfig)
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Failed to access GConfig!"));
		return false;
	}

	if (GConfig->LoadLocalIniFile(ConfigFile, *Filename, false, nullptr, false))
	{
		// Load each profile section
		bool bLoaded = true;

		// Load the _bAutomaticSwitch variable
		const FString SectionName = TEXT("HarmonyLink");
		const FString KeyName = TEXT("AutomaticProfileSwitch");

		if (!GConfig->GetBool(*SectionName, *KeyName, _bAutomaticSwitch, Filename))
		{
			UE_LOG(LogHarmonyLink, Error, TEXT("Failed to load bAutomaticSwitch from config"));
			bLoaded = false;
		}
		else
		{
			UE_LOG(LogHarmonyLink, Log, TEXT("Loaded bAutomaticSwitch: %s"), _bAutomaticSwitch ? TEXT("true") : TEXT("false"));
		}

		for (const TPair<EProfile, FName>& Profile : _ProfileNames)
		{
			if (!LoadSection(ConfigFile, Profile))
			{
				UE_LOG(LogHarmonyLink, Error, TEXT("Failed to load section: '%s'"), *Profile.Value.ToString());
				bLoaded = false;
			}
		}

		// Check if all profiles and settings were loaded successfully
		if (bLoaded)
		{
			UE_LOG(LogHarmonyLink, Log, TEXT("Successfully loaded config file: %s"), *Filename);
			return true;
		}
	}

	UE_LOG(LogHarmonyLink, Error, TEXT("Failed to load config file: %s"), *Filename);
	return false;
}

bool UHarmonyLinkGraphics::LoadSection(const FConfigFile& ConfigFile, const TPair<EProfile, FName> Profile)
{
	const FName& SectionName = Profile.Value;
	const EProfile ProfileKey = Profile.Key;

	if (const FConfigSection* Section = ConfigFile.FindSection(*SectionName.ToString()))
	{
		FSettingsProfile& SettingsProfile = _Profiles.FindOrAdd(ProfileKey);
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
	Intermal_SaveConfig(false);
}

void UHarmonyLinkGraphics::SetSetting(const EProfile Profile, const FName Setting, const FHLConfigValue Value)
{
	// Ignore if HarmonyLinkSettings is disabled
	if (Profile == EProfile::NONE)
	{
		return;
	}
	
	// Find the profile name associated with the given EProfile
	const FName* ProfileName = _ProfileNames.Find(Profile);
	
	if (!ProfileName)
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Profile not found."));
		return;
	}

	FString TypeString;
	FString ValueString;

	switch (Value.GetType())
	{
	case EConfigValueType::Int:
		ValueString = FString::FromInt(Value.GetValue<int32>());
		TypeString = TEXT("int");
		break;
	case EConfigValueType::Float:
		ValueString = FString::SanitizeFloat(Value.GetValue<float>());
		TypeString = TEXT("float");
		break;
	case EConfigValueType::Bool:
		ValueString = Value.GetValue<bool>() ? TEXT("true") : TEXT("false");
		TypeString = TEXT("bool");
		break;
	case EConfigValueType::String:
		ValueString = Value.GetValue<FString>();
		TypeString = TEXT("string");
		break;
	}

	UE_LOG(LogHarmonyLink, Log, TEXT("Applying '%s': Value='%s', Type='%s' to profile '%s'."), *Setting.ToString(), *TypeString, *ValueString, *ProfileName->ToString());
		
	// Find the settings associated with the profile
	FSettingsProfile* SettingsProfile = _Profiles.Find(Profile);

	if (!SettingsProfile)
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("No settings found for profile %s."), *ProfileName->ToString());
		return;
	}

	SettingsProfile->Settings.FindOrAdd(Setting) = Value;
}


UHarmonyLinkGraphics* UHarmonyLinkGraphics::GetSettings()
{
	// Check if we already initialised
	if (_INSTANCE)
	{
		return _INSTANCE;
	}

	// Proceed to create a new singleton instance
	_INSTANCE = NewObject<UHarmonyLinkGraphics>();
	_INSTANCE->AddToRoot();
	_INSTANCE->Init();
	
	return _INSTANCE;
}

FSettingsProfile UHarmonyLinkGraphics::GetSettingProfile(const EProfile Profile)
{
	// Ignore if HarmonyLinkSettings is disabled
	if (Profile == EProfile::NONE)
	{
		return FSettingsProfile();
	}
	
	// Find the profile name associated with the given EProfile
	const FName* ProfileName = _ProfileNames.Find(Profile);
	
	if (!ProfileName)
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Profile not found."));
		return FSettingsProfile();
	}

	// Find the settings associated with the profile
	FSettingsProfile* SettingsProfile = _Profiles.Find(Profile);

	if (!SettingsProfile)
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("No settings found for profile %s."), *ProfileName->ToString());
		return FSettingsProfile();
	}

	return *SettingsProfile;
}

EProfile UHarmonyLinkGraphics::GetActiveProfile() const
{
	return _ActiveProfile;
}

void UHarmonyLinkGraphics::SetAutomaticSwitching(const bool bAutomaticSwitch)
{
	_bAutomaticSwitch = bAutomaticSwitch;
	OnAutomaticSwitchChanged.Broadcast(_bAutomaticSwitch);
}

bool UHarmonyLinkGraphics::GetAutomaticSwitching() const
{
	return _bAutomaticSwitch;
}

void UHarmonyLinkGraphics::DestroySettings()
{
	if (_INSTANCE)
	{
		UE_LOG(LogHarmonyLink, Log, TEXT("Destroying UHarmonyLinkGraphics."))
		FWorldDelegates::OnPostWorldInitialization.RemoveAll(_INSTANCE);
		FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(_INSTANCE);
		_INSTANCE->RemoveFromRoot();
		_INSTANCE->MarkAsGarbage();
		_INSTANCE = nullptr;
	}
}

void UHarmonyLinkGraphics::Init()
{
	LoadConfig();

	const FBattery BatteryStatus = UHarmonyLinkLibrary::GetBatteryStatus();
	
	if (BatteryStatus.HasBattery)
	{
		ApplyProfileInternal(EProfile::BATTERY);
	}
}

void UHarmonyLinkGraphics::Intermal_SaveConfig(const bool bDefaultConfig) const
{
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_SaveConfig);

	const FString Filename = GetConfigDirectoryFile(bDefaultConfig);
	const FString SectionName = TEXT("HarmonyLink");
	const FString KeyName = TEXT("AutomaticProfileSwitch");

	// Save the _bAutomaticSwitch variable
	GConfig->SetBool(*SectionName, *KeyName, _bAutomaticSwitch, Filename);

	UE_LOG(LogHarmonyLink, Log, TEXT("Saving bAutomaticSwitch: %s"), _bAutomaticSwitch ? TEXT("true") : TEXT("false"));

	for (const TPair<EProfile, FSettingsProfile>& Profile : _Profiles)
	{
		SaveSection(Profile.Value, bDefaultConfig);
	}
	
	UE_LOG(LogHarmonyLink, Log, TEXT("Flushing file: '%s'"), *Filename);
	GConfig->Flush(true, Filename);
}

void UHarmonyLinkGraphics::Tick()
{
	Async(EAsyncExecution::Thread, [this]()
	{
		const FBattery BatteryStatus = UHarmonyLinkLibrary::GetBatteryStatus();

		if (BatteryStatus.BatteryPercent != _LastBatteryPercentage)
		{
			// Ensure thread-safe broadcasting
			Async(EAsyncExecution::TaskGraphMainThread, [this, BatteryStatus]()
			{
				OnBatteryLevelChanged.Broadcast(BatteryStatus.BatteryPercent);
			});
		}

		if (!_bAutomaticSwitch)
		{
			return;
		}

		if (BatteryStatus.HasBattery)
		{
			if (BatteryStatus.IsACConnected)
			{
				if (_ActiveProfile != EProfile::CHARGING)
				{
					Async(EAsyncExecution::TaskGraphMainThread, [this]()
					{
						ApplyProfileInternal(EProfile::CHARGING);
					});
				}
			}
			else
			{
				if (_ActiveProfile != EProfile::BATTERY)
				{
					Async(EAsyncExecution::TaskGraphMainThread, [this]()
					{
						ApplyProfileInternal(EProfile::BATTERY);
					});
				}
			}
		}
	});
}

void UHarmonyLinkGraphics::CreateDefaultConfigFile()
{
	UE_LOG(LogHarmonyLink, Log, TEXT("Creating default config file."));

	LoadDefaults();
	Intermal_SaveConfig(true);
	
	UE_LOG(LogHarmonyLink, Log, TEXT("Default config file created."));
}

FString UHarmonyLinkGraphics::GetConfigDirectoryFile(const bool bDefaultFolder)
{
	FString ConfigFileName = bDefaultFolder? "DefaultHarmonyLink.ini" : "HarmonyLink.ini"; // Replace with your actual config file name

	return FPaths::Combine(bDefaultFolder ? FPaths::ProjectConfigDir() : FPaths::GeneratedConfigDir(), ConfigFileName);
}

void UHarmonyLinkGraphics::SaveSection(const FSettingsProfile& SettingsProfile, const bool bDefaultConfig, const bool bFlush)
{
	if (GConfig)
	{
		const FString Filename = GetConfigDirectoryFile(bDefaultConfig);
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

	_Profiles.Reset();

	// Iterate over ProfileNames to create default settings
	for (const TPair<EProfile, FName>& Profile : _ProfileNames)
	{
		FSettingsProfile NewProfileSettings;
		NewProfileSettings.SectionName = Profile.Value;

		if (const TMap<FName, FHLConfigValue>* Settings = _DefaultSettings.Find(Profile.Value))
		{
			NewProfileSettings.Settings = *Settings;
		}

		_Profiles.Add(Profile.Key, NewProfileSettings);
	}
}

bool UHarmonyLinkGraphics::ApplyProfileInternal(const EProfile Profile)
{
	// If the profile is None, revert to the original user game settings
	if (Profile == EProfile::NONE)
	{
		UE_LOG(LogHarmonyLink, Log, TEXT("Reverting to original user game settings."));

		if (UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
		{
			UserSettings->LoadSettings(true);
			UserSettings->ApplySettings(true);
			_ActiveProfile = EProfile::NONE;
			OnProfileChanged.Broadcast(_ActiveProfile);
			UE_LOG(LogHarmonyLink, Log, TEXT("Original user game settings applied."));
			return true;
		}
		
		UE_LOG(LogHarmonyLink, Warning, TEXT("Failed to get user game settings."));
		return false;
	}

	
	// Find the profile name associated with the given EProfile
	const FName* ProfileName = _ProfileNames.Find(Profile);
	
	if (!ProfileName)
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Profile not found."));
		return false;
	}

	UE_LOG(LogHarmonyLink, Log, TEXT("Applying profile %s."), *ProfileName->ToString());
		
	// Find the settings associated with the profile
	FSettingsProfile* SettingsProfile = _Profiles.Find(Profile);

	if (!SettingsProfile)
	{
		UE_LOG(LogHarmonyLink, Warning, TEXT("No settings found for profile %s."), *ProfileName->ToString());
		return false;
	}

	{
		FGlobalComponentRecreateRenderStateContext Context;

		// Example of applying settings (actual application depends on your implementation)
		for (const TPair<FName, FHLConfigValue>& Setting : SettingsProfile->Settings)
		{
			// Example of logging each setting being applied
			UE_LOG(LogHarmonyLink, Log, TEXT("Patching CVar override: %s = %s"),
				   *Setting.Key.ToString(), *Setting.Value.ToString());

			ApplySetting(Setting);
		}
	}

	_ActiveProfile = Profile;
	OnProfileChanged.Broadcast(_ActiveProfile);
	return true;
}

void UHarmonyLinkGraphics::OnPostWorldInitialization(UWorld* World, UWorld::InitializationValues IVS)
{
	if (!World || !World->IsValidLowLevel())
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Failed to Hook into World Initialisation!"))
		return;
	}
	
	if (World->IsGameWorld())
	{
		if (_INSTANCE)
		{
			FTimerManager* TimerManager = &World->GetTimerManager();
			if (!TimerManager)
			{
				UE_LOG(LogHarmonyLink, Error, TEXT("Failed get TimerManager!"))
				return;
			}
			
			if (!TimerManager->TimerExists(_INSTANCE->_TickTimerHandle) || !TimerManager->IsTimerActive(_INSTANCE->_TickTimerHandle))
			{
				World->GetTimerManager().SetTimer(_INSTANCE->_TickTimerHandle, [&, TimerManager]
				{
					if (!this)
					{
						UE_LOG(LogHarmonyLink, Error, TEXT("'This' is destroyed, Clearing timer."))
						if (TimerManager)
						{
							TimerManager->ClearTimer(_TickTimerHandle);
						}
						return;
					}
					Tick();
				}, _TickRate, true);
			}
		}
	}
}

void UHarmonyLinkGraphics::OnWorldEnd(UWorld* World)
{
	if (!World || !World->IsValidLowLevel())
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("World Already destroyed"))
		return;
	}
	
	if(World->GetTimerManager().TimerExists(_TickTimerHandle))
	{
		World->GetTimerManager().ClearTimer(_TickTimerHandle);
	}

	// Ensure we safely destroy our singleton instance
	DestroySettings();
}

bool UHarmonyLinkGraphics::ApplyProfile(const EProfile Profile, const bool bDisableAutomaticSwitch)
{
	// Manual profile change, turn off automatic switching
	if (bDisableAutomaticSwitch)
	{
		_bAutomaticSwitch = false;
	}
	
	return ApplyProfileInternal(Profile);
}

void UHarmonyLinkGraphics::ApplySetting(const TPair<FName, FHLConfigValue>& Setting)
{
	// Apply the setting based on the key (CVar)
	IConsoleManager& ConsoleManager = IConsoleManager::Get();
	IConsoleVariable* CVar = ConsoleManager.FindConsoleVariable(*Setting.Key.ToString());

	if (CVar)
	{
		switch (Setting.Value.GetType())
		{
		case EConfigValueType::Bool:
			CVar->Set(Setting.Value.GetValue<bool>(), ECVF_SetByGameSetting);
			break;
                
		case EConfigValueType::Float:
			CVar->Set(Setting.Value.GetValue<float>(), ECVF_SetByGameSetting);
			break;
                
		case EConfigValueType::Int:
			CVar->Set(Setting.Value.GetValue<int32>(), ECVF_SetByGameSetting);
			break;
                
		default:
			UE_LOG(LogHarmonyLink, Warning, TEXT("Unsupported value type for setting: %s"), *Setting.Key.ToString());
			break;
		}
	}
	else
	{
		UE_LOG(LogHarmonyLink, Warning, TEXT("Console variable not found: %s"), *Setting.Key.ToString());
	}
}

void UHarmonyLinkGraphics::DebugPrintProfiles() const
{
	UE_LOG(LogHarmonyLink, Log, TEXT("DebugPrintProfiles started."));

	for (TPair<EProfile, FSettingsProfile> Profile : _Profiles)
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
	_INSTANCE->DestroySettings();
	GetSettings();
}
