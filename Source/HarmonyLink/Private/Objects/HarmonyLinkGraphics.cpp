// Copyright (C) 2024 Jordon Brooks


#include "Objects/HarmonyLinkGraphics.h"
#include "ComponentRecreateRenderStateContext.h"
#include "HarmonyLink.h"
#include "HarmonyLinkLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "HarmonyLinkLib.h"

UHarmonyLinkGraphics* UHarmonyLinkGraphics::_INSTANCE = nullptr;
int32 UHarmonyLinkGraphics::_TickRate = 1;
FTimerHandle UHarmonyLinkGraphics::_TickTimerHandle = FTimerHandle();
TSharedPtr<FConfigFile> UHarmonyLinkGraphics::_ConfigFile = nullptr;
bool UHarmonyLinkGraphics::_bAutomaticSwitch = false;
int32 UHarmonyLinkGraphics::_LastBatteryPercentage = 0;
EProfile UHarmonyLinkGraphics::_ActiveProfile = EProfile::NONE;
TMap<EProfile, FSettingsProfile> UHarmonyLinkGraphics::_Profiles = TMap<EProfile, FSettingsProfile>();

TMap<EProfile, FName> UHarmonyLinkGraphics::_ProfileNames =
{
	{EProfile::BATTERY, "Battery"},
	{EProfile::CHARGING, "Charging"},
	{EProfile::DOCKED, "Docked"},
};

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
//		            { TEXT("r.ReflectionMethod"), FHLConfigValue(0) },
//					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
//					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(0) },
//					{ TEXT("r.ScreenPercentage"), FHLConfigValue(50) },
	}},
	{ "Charging", {
//		            { TEXT("r.ReflectionMethod"), FHLConfigValue(2) },
//					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
//					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(1) },
//					{ TEXT("r.ScreenPercentage"), FHLConfigValue(100) },
	}},
	{ "Docked",	{
//		            { TEXT("r.ReflectionMethod"), FHLConfigValue(2) },
//					{ TEXT("r.DynamicGlobalIlluminationMethod"), FHLConfigValue(2) },
//					{ TEXT("r.Shadow.Virtual.Enable"), FHLConfigValue(1) },
//					{ TEXT("r.ScreenPercentage"), FHLConfigValue(100) },
	}}
};

UHarmonyLinkGraphics::UHarmonyLinkGraphics()
{
	UE_LOG(LogHarmonyLink, Warning, TEXT("HarmonyLinkGraphics initialized."));
	if (_INSTANCE != this)
	{
		if (_INSTANCE)
		{
			DestroySettings();
		}

		_INSTANCE = this;
	}

	AddToRoot();
	
	FWorldDelegates::OnPostWorldInitialization.AddStatic(&UHarmonyLinkGraphics::OnPostWorldInitialization);
	FWorldDelegates::OnPreWorldFinishDestroy.AddStatic(&UHarmonyLinkGraphics::OnWorldEnd);

	Init();
}

UHarmonyLinkGraphics::~UHarmonyLinkGraphics()
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("~UHarmonyLinkGraphics called."));
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(this);
}

void UHarmonyLinkGraphics::LoadConfig(const bool bForceReload)
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("LoadConfig called."));
	QUICK_SCOPE_CYCLE_COUNTER(HarmonyLinkGraphics_LoadSettings);

	// Load the settings into the map
	GetConfig();

	DebugPrintProfiles();
}

bool UHarmonyLinkGraphics::LoadSettingsFromConfig(FConfigFile* ConfigFile) const
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("LoadSettingsFromConfig called."));
	//const FString Filename = "HarmonyLink"; //GetConfigDirectoryFile(bLoadDefaults);
	
	// Load each profile section
	bool bLoaded = true;

	// Load the _bAutomaticSwitch variable
	const FString SectionName = TEXT("HarmonyLink");
	const FString KeyName = TEXT("AutomaticProfileSwitch");

	if (!ConfigFile->GetBool(*SectionName, *KeyName, _bAutomaticSwitch))
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
		UE_LOG(LogHarmonyLink, Log, TEXT("Successfully loaded config."));
		return true;
	}

	UE_LOG(LogHarmonyLink, Error, TEXT("Failed to load config file."));
	return false;
}

bool UHarmonyLinkGraphics::LoadSection(FConfigFile* ConfigFile, const TPair<EProfile, FName> Profile)
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("LoadSection called."));
	if (!ensureMsgf(ConfigFile, TEXT("ConfigFile is nullptr!"))) return false;
	
	const FName& SectionName = Profile.Value;
	const EProfile ProfileKey = Profile.Key;

	const FConfigSection* Section = nullptr;


#if (ENGINE_MAJOR_VERSION >= 5)
	Section = ConfigFile->FindSection(*SectionName.ToString())
#elif (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION >= 27)
	Section = ConfigFile->FindOrAddSection(*SectionName.ToString());
#else
	#error "Unsupported Unreal Engine version"
#endif

	if (Section)
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
	UE_LOG(LogHarmonyLink, Verbose, TEXT("SaveConfig called."));
	Intermal_SaveConfig(false);
}

void UHarmonyLinkGraphics::SetSetting(const EProfile Profile, const FName Setting, const FHLConfigValue Value)
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("SetSetting called."));
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
		ValueString = Value.GetValue<bool>() ? TEXT("True") : TEXT("False");
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
	UE_LOG(LogHarmonyLink, VeryVerbose, TEXT("GetSettings called."));
	// Check if we already initialised
	if (_INSTANCE)
	{
		return _INSTANCE;
	}

	// Proceed to create a new singleton instance
	_INSTANCE = NewObject<UHarmonyLinkGraphics>();
	
	return _INSTANCE;
}

FSettingsProfile UHarmonyLinkGraphics::GetSettingProfile(const EProfile Profile)
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("GetSettingProfile called."));
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
	UE_LOG(LogHarmonyLink, Verbose, TEXT("GetActiveProfile called."));
	return _ActiveProfile;
}

void UHarmonyLinkGraphics::SetAutomaticSwitching(const bool bAutomaticSwitch)
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("SetAutomaticSwitching called."));
	_bAutomaticSwitch = bAutomaticSwitch;
	OnAutomaticSwitchChanged.Broadcast(_bAutomaticSwitch);
}

bool UHarmonyLinkGraphics::GetAutomaticSwitching() const
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("GetAutomaticSwitching called."));
	return _bAutomaticSwitch;
}

void UHarmonyLinkGraphics::DestroySettings()
{
	UE_LOG(LogHarmonyLink, Log, TEXT("DestroySettings called."));
	if (_INSTANCE)
	{
		UE_LOG(LogHarmonyLink, Log, TEXT("Destroying UHarmonyLinkGraphics."))
		FWorldDelegates::OnPostWorldInitialization.RemoveAll(_INSTANCE);
		FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(_INSTANCE);
		_INSTANCE->RemoveFromRoot();
		
#if (ENGINE_MAJOR_VERSION >= 5)
		 _INSTANCE->MarkAsGarbage(); // For UE 5.0 and above
#elif (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION >= 27)
		_INSTANCE->MarkPendingKill(); // For UE 4.27 and above
#else
#error "Unsupported Unreal Engine version"
#endif
		
		_INSTANCE = nullptr;
	}
}

void UHarmonyLinkGraphics::Init()
{
	UE_LOG(LogHarmonyLink, Log, TEXT("Init called."));

	if (!HarmonyLinkLib::HL_Init())
	{
		UE_LOG(LogHarmonyLink, Fatal, TEXT("Failed to initialise HarmonyLinkLib!"));
		return;
	}

	LoadConfig();

	const FBattery BatteryStatus = UHarmonyLinkLibrary::GetBatteryStatus();

	if (BatteryStatus.HasBattery)
	{
		if (BatteryStatus.IsACConnected)
		{
			ApplyProfileInternal(EProfile::BATTERY);
		}
		else
		{
			ApplyProfileInternal(EProfile::CHARGING);
		}
	}
}

void UHarmonyLinkGraphics::Intermal_SaveConfig(const bool bDefaultConfig) const
{
	UE_LOG(LogHarmonyLink, Log, TEXT("Intermal_SaveConfig called."));
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
	GetConfig()->Dirty = true;
	// You'd think that Write would actually write something but for some
	// reason even if it outputs a success the file doesn't actually get created.
	// For this reason, Flush seems to work for now.
	//GetConfig()->Write(Filename);
	GConfig->Flush(true, Filename);
}

void UHarmonyLinkGraphics::Tick()
{
	UE_LOG(LogHarmonyLink, VeryVerbose, TEXT("Tick called."));
	const FBattery BatteryStatus = UHarmonyLinkLibrary::GetBatteryStatus();

	if (BatteryStatus.BatteryPercent != _LastBatteryPercentage)
	{
		// Ensure thread-safe broadcasting
		OnBatteryLevelChanged.Broadcast(BatteryStatus.BatteryPercent);
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
				ApplyProfileInternal(EProfile::CHARGING);
			}
		}
		else
		{
			if (_ActiveProfile != EProfile::BATTERY)
			{
				ApplyProfileInternal(EProfile::BATTERY);
			}
		}
	}
}

void UHarmonyLinkGraphics::CreateDefaultConfigFile() const
{
	UE_LOG(LogHarmonyLink, Log, TEXT("CreateDefaultConfigFile called."));
	UE_LOG(LogHarmonyLink, Log, TEXT("Creating default config file."));

	LoadDefaults();
	Intermal_SaveConfig(true);
	
	UE_LOG(LogHarmonyLink, Log, TEXT("Default config file created."));
}

FString UHarmonyLinkGraphics::GetConfigDirectoryFile(const bool bDefaultFolder)
{
	UE_LOG(LogHarmonyLink, Log, TEXT("GetConfigDirectoryFile called."));
	FString ConfigFileName = bDefaultFolder ? TEXT("DefaultHarmonyLink.ini") : TEXT("HarmonyLink.ini");

	FString ConfigDirectory = bDefaultFolder ? FPaths::ProjectConfigDir() : FPaths::Combine(FPaths::GeneratedConfigDir(), UGameplayStatics::GetPlatformName());

	return FPaths::Combine(ConfigDirectory, ConfigFileName);
}

void UHarmonyLinkGraphics::SaveSection(const FSettingsProfile& SettingsProfile, const bool bDefaultConfig, const bool bFlush) const
{
	UE_LOG(LogHarmonyLink, Log, TEXT("SaveSection called."));
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
				ValueString = Setting.Value.GetValue<bool>() ? TEXT("True") : TEXT("False");
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

void UHarmonyLinkGraphics::LoadDefaults() const
{
	UE_LOG(LogHarmonyLink, Log, TEXT("LoadDefaults called."));

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
	UE_LOG(LogHarmonyLink, Log, TEXT("ApplyProfileInternal called."));
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
	UE_LOG(LogHarmonyLink, Log, TEXT("OnPostWorldInitialization called."));
	if (!World || !World->IsValidLowLevel())
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Failed to Hook into World Initialisation!"))
		return;
	}
	
	if (World->IsGameWorld())
	{
		if (IsValid(GetSettings()))
		{
			FTimerManager* TimerManager = &World->GetTimerManager();
			if (!TimerManager)
			{
				UE_LOG(LogHarmonyLink, Error, TEXT("Failed get TimerManager!"))
				return;
			}
			
			if (!TimerManager->TimerExists(_TickTimerHandle) || !TimerManager->IsTimerActive(_TickTimerHandle))
			{

				World->GetTimerManager().SetTimer(_TickTimerHandle, [TimerManager]
				{
					if (!GetSettings())
					{
						UE_LOG(LogHarmonyLink, Error, TEXT("'This' is destroyed, Clearing timer."))
						if (TimerManager)
						{
							TimerManager->ClearTimer(_TickTimerHandle);
						}
						return;
					}
					GetSettings()->Tick();
				}, _TickRate, true);
			}
			else
			{
				UE_LOG(LogHarmonyLink, Error, TEXT("Error: Timer already exists."));
			}
		}
		else
		{
			UE_LOG(LogHarmonyLink, Error, TEXT("'This' is nullptr!"));
		}
	}
	else
	{
		//UE_LOG(LogHarmonyLink, Error, TEXT("Failed to bring up tick!"));
	}
}

void UHarmonyLinkGraphics::OnWorldEnd(UWorld* World)
{
	UE_LOG(LogHarmonyLink, Log, TEXT("OnWorldEnd(UWorld* World) called."));
	if (!World)
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
	UE_LOG(LogHarmonyLink, Log, TEXT("Applying Profile."));
	// Manual profile change, turn off automatic switching
	if (bDisableAutomaticSwitch)
	{
		_bAutomaticSwitch = false;
	}
	
	return ApplyProfileInternal(Profile);
}

void UHarmonyLinkGraphics::ApplySetting(const TPair<FName, FHLConfigValue>& Setting)
{
	UE_LOG(LogHarmonyLink, Log, TEXT("Applying settings."));
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

FConfigFile* UHarmonyLinkGraphics::GetConfig() const
{
	UE_LOG(LogHarmonyLink, Verbose, TEXT("GetConfig Called."));
	if (_ConfigFile)
	{
		return _ConfigFile.Get();
	}

	FConfigFile* ConfigFile = GConfig->Find(GetConfigDirectoryFile(false), false);
	
	if (!ConfigFile)
	{
		UE_LOG(LogHarmonyLink, Warning, TEXT("Config file not found, attempting to read DefaultHarmonyLink.ini."));
		// Look in ProjectFolder->Config->DefaultHarmonyLink.ini
		ConfigFile = GConfig->Find(GetConfigDirectoryFile(true), true);
	}

	/*if (!ConfigFile)
	{
		CreateDefaultConfigFile();
		ConfigFile = GConfig->Find(GetConfigDirectoryFile(true), true);
	}*/

	if (ConfigFile)
	{
		UE_LOG(LogHarmonyLink, Verbose, TEXT("Setting up config."));
		ConfigFile->Name = "HarmonyLink";
		LoadSettingsFromConfig(ConfigFile);
		_ConfigFile = MakeShareable(ConfigFile);
	}
	else
	{
		UE_LOG(LogHarmonyLink, Error, TEXT("Failed to make config variable!"))
		return nullptr;
	}
	
	return _ConfigFile.Get();
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
	UE_LOG(LogHarmonyLink, Log, TEXT("Resetting instance."));
	_INSTANCE->DestroySettings();
	GetSettings();
}
