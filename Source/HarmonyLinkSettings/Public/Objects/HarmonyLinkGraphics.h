// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "Enums/Profile.h"
#include "Structs/SettingsProfile.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HarmonyLinkGraphics.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProfileChanged, EProfile, Profile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutomaticSwitchChanged, bool, bAutomaticSwich);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryLevelChanged, int32, BatteryPercent);

/**
 * 
 */
UCLASS(Blueprintable, config="HarmonyLink")
class HARMONYLINKSETTINGS_API UHarmonyLinkGraphics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	virtual ~UHarmonyLinkGraphics() override;

	UPROPERTY(BlueprintAssignable)
	FOnProfileChanged OnProfileChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAutomaticSwitchChanged OnAutomaticSwitchChanged;

	UPROPERTY(BlueprintAssignable)
	FOnBatteryLevelChanged OnBatteryLevelChanged;

	/**
	 * @brief Loads the graphics configuration settings.
	 * 
	 * This function loads the graphics settings into the appropriate map. If the initial loading attempt fails,
	 * it retries a second time. The settings are loaded using the `LoadSettingsFromConfig` method. 
	 * After loading the settings, the function prints debug information about the profiles.
	 * 
	 * @param bForceReload Indicates whether to force reload the settings. Defaults to false.
	 * 
	 * @note This function uses the QUICK_SCOPE_CYCLE_COUNTER macro for performance profiling.
	 */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void LoadConfig(const bool bForceReload = false);

	/**
	 * @brief Saves the current graphics configuration settings to the configuration file.
	 * 
	 * This function iterates through all profiles and saves their settings to the configuration file.
	 * Additionally, it saves the `_bAutomaticSwitch` variable to the configuration file and flushes the changes.
	 * 
	 * @details 
	 * - Uses the QUICK_SCOPE_CYCLE_COUNTER macro for performance profiling.
	 * - Iterates through the `_Profiles` map and calls `SaveSection` for each profile to save its settings.
	 * - Saves the `_bAutomaticSwitch` variable under the "HarmonyLink" section with the key "AutomaticProfileSwitch".
	 * - Logs the status of `_bAutomaticSwitch` and flushes the configuration file to ensure all changes are written.
	 * 
	 * @note Uses UE_LOG for logging the save process and status messages.
	 */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SaveConfig() const;

	/**
	 * @brief Sets a specific configuration value for a given profile.
	 * 
	 * This function updates a setting for a specified profile with a new value. It handles various data types (int, float, bool, string)
	 * and logs the process. If the profile or settings cannot be found, it logs an error message.
	 * 
	 * @param Profile The profile identifier for which the setting is to be updated.
	 * @param Setting The name of the setting to update.
	 * @param Value The new value to set for the specified setting.
	 * 
	 * @details 
	 * - Finds the profile name associated with the given `EProfile`.
	 * - Determines the type of the value and converts it to a string.
	 * - Logs the application of the setting with its value and type.
	 * - Finds the settings profile associated with the given profile.
	 * - Updates the setting with the new value or adds it if it does not exist.
	 * 
	 * @note Uses UE_LOG for logging the update process and any errors.
	 */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SetSetting(EProfile Profile, FName Setting, const FHLConfigValue& Value);

	/**
	 * @brief Applies the specified graphics profile.
	 * 
	 * This function applies the given graphics profile. If automatic profile switching is enabled, it can be disabled by setting the `bDisableAutomaticSwitch` parameter to true.
	 * 
	 * @param Profile The profile to apply.
	 * @param bDisableAutomaticSwitch Indicates whether to disable automatic profile switching. Defaults to true.
	 * @return bool Returns true if the profile was successfully applied; false otherwise.
	 * 
	 * @details 
	 * - If `bDisableAutomaticSwitch` is true, sets `_bAutomaticSwitch` to false to disable automatic profile switching.
	 * - Calls `ApplyProfileInternal` to apply the specified profile.
	 * - Returns the result of `ApplyProfileInternal`.
	 */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	bool ApplyProfile(EProfile Profile, const bool bDisableAutomaticSwitch = true);

	/**
	 * @brief Retrieves the singleton instance of the UHarmonyLinkGraphics settings.
	 * 
	 * This function returns the singleton instance of the `UHarmonyLinkGraphics` class. If the instance has not been
	 * initialized, it creates a new instance, adds it to the root to prevent garbage collection, initializes it, and
	 * then returns the instance.
	 * 
	 * @return UHarmonyLinkGraphics* The singleton instance of the `UHarmonyLinkGraphics` class.
	 * 
	 * @details 
	 * - Checks if the singleton instance (`_INSTANCE`) is already initialized.
	 * - If not, creates a new instance using `NewObject<UHarmonyLinkGraphics>`.
	 * - Adds the new instance to the root to prevent it from being garbage collected.
	 * - Calls the `Init` function on the new instance to perform any necessary initialization.
	 * - Returns the singleton instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink Settings")
	static UHarmonyLinkGraphics* GetSettings();

	/**
	 * @brief Retrieves the settings profile for a specified profile.
	 * 
	 * This function returns the settings profile associated with the given `EProfile`. If the profile name or the settings
	 * cannot be found, it logs an error message and returns an empty `FSettingsProfile`.
	 * 
	 * @param Profile The profile identifier for which the settings profile is to be retrieved.
	 * @return FSettingsProfile The settings profile associated with the specified profile. If the profile is not found, returns an empty `FSettingsProfile`.
	 * 
	 * @details 
	 * - Finds the profile name associated with the given `EProfile`.
	 * - Logs an error and returns an empty `FSettingsProfile` if the profile name is not found.
	 * - Finds the settings profile associated with the profile.
	 * - Logs an error and returns an empty `FSettingsProfile` if the settings profile is not found.
	 * - Returns the settings profile if found.
	 * 
	 * @note Uses UE_LOG for logging errors.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintPure, Category="HarmonyLink Settings")
	FSettingsProfile GetSettingProfile(const EProfile Profile);

	/**
	 * @brief Retrieves the currently active profile.
	 * 
	 * This function returns the currently active profile of the `UHarmonyLinkGraphics` settings.
	 * 
	 * @return EProfile The currently active profile.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink Settings")
	EProfile GetActiveProfile() const;

	/**
	 * @brief Sets the automatic profile switching state.
	 * 
	 * This function updates the automatic profile switching setting and broadcasts the change.
	 * 
	 * @param bAutomaticSwitch Indicates whether automatic profile switching should be enabled (true) or disabled (false).
	 * 
	 * @details 
	 * - Updates the `_bAutomaticSwitch` member variable with the new value.
	 * - Broadcasts the change to any listeners using the `OnAutomaticSwitchChanged` delegate.
	 */
	UFUNCTION(BlueprintCallable, Category="HarmonyLink Settings")
	void SetAutomaticSwitching(const bool bAutomaticSwitch);

	/**
	 * @brief Retrieves the current state of automatic profile switching.
	 * 
	 * This function returns the state of the automatic profile switching setting.
	 * 
	 * @return bool True if automatic profile switching is enabled; false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HarmonyLink Settings")
	bool GetAutomaticSwitching() const;

	/**
	 * @brief Destroys the singleton instance of the UHarmonyLinkGraphics settings.
	 * 
	 * This function safely destroys the singleton instance of the `UHarmonyLinkGraphics` class by removing it from root,
	 * marking it as garbage for cleanup, and setting the instance pointer to null.
	 * 
	 * @details 
	 * - Checks if the singleton instance (`_INSTANCE`) exists.
	 * - Logs the destruction of the `UHarmonyLinkGraphics` instance.
	 * - Removes all delegate bindings from `_INSTANCE`.
	 * - Removes `_INSTANCE` from the root to allow garbage collection.
	 * - Marks `_INSTANCE` as garbage to enable cleanup.
	 * - Sets `_INSTANCE` to null.
	 * 
	 * @note Uses UE_LOG for logging the destruction process.
	 */
	static void DestroySettings();

private:
	/**
	 * @brief Initializes the UHarmonyLinkGraphics settings.
	 * 
	 * This function initializes the `UHarmonyLinkGraphics` settings by loading the configuration and applying the
	 * appropriate profile based on the battery status.
	 * 
	 * @details 
	 * - Calls `LoadConfig` to load the configuration settings.
	 * - Retrieves the current battery status using `UHarmonyLinkLibrary::GetBatteryStatus`.
	 * - If the device has a battery, applies the `BATTERY` profile by calling `ApplyProfileInternal`.
	 */
	void Init();

	void Intermal_SaveConfig(const bool bDefaultConfig) const;

	/**
	 * @brief Periodically checks and updates the battery status and profile settings.
	 * 
	 * This function is executed based on a timer determined by `_TickRate`. It asynchronously checks the current battery status
	 * and updates the battery percentage and profile settings as needed. If the battery percentage has changed, it broadcasts
	 * this change. If automatic profile switching is enabled, it applies the appropriate profile based on the AC connection status.
	 * 
	 * @details 
	 * - Runs asynchronously on a separate thread to fetch the current battery status using `UHarmonyLinkLibrary::GetBatteryStatus`.
	 * - If the battery percentage has changed, broadcasts the new battery percentage in a thread-safe manner.
	 * - If automatic profile switching (`_bAutomaticSwitch`) is disabled, exits the function.
	 * - If the device has a battery:
	 *   - Checks the AC connection status and applies the `CHARGING` profile if connected to AC.
	 *   - Applies the `BATTERY` profile if not connected to AC.
	 * - Ensures all profile applications and broadcasts are executed on the main thread for thread safety.
	 * 
	 * @note Uses UE_LOG for logging and `Async` for asynchronous execution.
	 */
	void Tick();
	
	/**
	 * @brief Creates a default configuration file.
	 * 
	 * This function creates a default configuration file by loading the default settings and then saving them to the config file.
	 * 
	 * @details 
	 * - Logs the start of the default configuration file creation process.
	 * - Calls `LoadDefaults` to load the default settings into the configuration.
	 * - Calls `SaveConfig` to save the default settings to the configuration file.
	 * - Logs the completion of the default configuration file creation process.
	 * 
	 * @note Uses UE_LOG for logging the creation process.
	 */
	void CreateDefaultConfigFile() const;

	/**
	 * @brief Retrieves the path to the configuration file.
	 * 
	 * This function constructs and returns the full path to the configuration file used by the `UHarmonyLinkGraphics` settings.
	 * 
	 * @return FString The full path to the configuration file.
	 * 
	 * @details 
	 * - Defines the configuration file name as "HarmonyLink.ini".
	 * - Combines the generated config directory path with the configuration file name to form the full path.
	 * - Returns the full path to the configuration file.
	 */
	static FString GetConfigDirectoryFile(const bool bDefaultFolder);

	/**
	 * @brief Loads the settings from the configuration file.
	 * 
	 * This function loads the graphics settings from an INI configuration file into the appropriate structures.
	 * It attempts to load each profile section and the automatic switch setting. If any section fails to load,
	 * it logs an error message and indicates failure. If loading is successful, it logs a success message.
	 * 
	 * @return true if the settings were successfully loaded; false otherwise.
	 * 
	 * @details 
	 * - Loads the configuration file specified by `GetConfigDirectoryFile`.
	 * - Iterates over the `_ProfileNames` to load each profile section.
	 * - Loads the `_bAutomaticSwitch` variable from the config.
	 * - If loading fails, it attempts to create a default configuration file.
	 * 
	 * @note Uses UE_LOG for logging errors and success messages.
	 */
	bool LoadSettingsFromConfig(FConfigFile* ConfigFile) const;

	/**
	 * @brief Loads a specific section from the configuration file into a settings profile.
	 * 
	 * This function extracts settings from a specified section of the configuration file and stores them in the corresponding profile.
	 * It handles different data types (int, float, bool, string) and parses them accordingly. If the section is found and loaded successfully,
	 * the settings are added to the profile's settings map.
	 * 
	 * @param ConfigFile The configuration file from which to load the section.
	 * @param Profile The profile key-value pair containing the profile enum and the section name.
	 * @return true if the section was found and loaded successfully; false otherwise.
	 * 
	 * @details 
	 * - Finds the section in the configuration file using the section name from the profile.
	 * - Clears any previous settings in the profile.
	 * - Iterates through the settings in the section, parsing the value and type, and adds them to the profile's settings map.
	 * 
	 * @note The function handles settings of types int, float, bool, and string.
	 */
	static bool LoadSection(FConfigFile* ConfigFile, const TPair<EProfile, FName> Profile);
	
	
	/**
	 * @brief Saves the settings of a given profile section to the configuration file.
	 * 
	 * This function saves the settings of a specified profile section to the configuration file. It handles various data types
	 * (int, float, bool, string) and formats them accordingly before saving. If the `bFlush` parameter is true, the configuration
	 * file is flushed to ensure all changes are written to disk.
	 * 
	 * @param SettingsProfile The settings profile to be saved.
	 * @param bFlush Indicates whether to flush the configuration file after saving. Defaults to false.
	 * 
	 * @details 
	 * - Checks if the global configuration object (`GConfig`) is available.
	 * - Constructs the full path to the configuration file using `GetConfigDirectoryFile`.
	 * - Iterates through the settings in the provided `SettingsProfile`.
	 * - Determines the type and value of each setting and formats them as strings.
	 * - Sets the formatted string in the configuration file for each setting.
	 * - Logs the save operation.
	 * - If `bFlush` is true, flushes the configuration file to ensure all changes are written.
	 * 
	 * @note Uses UE_LOG for logging the save and flush operations.
	 */
	void SaveSection(const FSettingsProfile& SettingsProfile, const bool bDefaultConfig, const bool bFlush = false) const;

	/**
	 * @brief Loads the default settings into the profiles.
	 * 
	 * This function resets the current profiles and loads the default settings into the profiles. It iterates over the profile names
	 * and assigns the default settings to each profile.
	 * 
	 * @details 
	 * - Logs the start of the `LoadDefaults` process.
	 * - Resets the `_Profiles` map to clear any existing settings.
	 * - Iterates over `_ProfileNames` to create default settings for each profile.
	 * - For each profile, initializes a `FSettingsProfile` with the section name.
	 * - If default settings are found for the profile in `_DefaultSettings`, assigns these settings to the profile.
	 * - Adds the newly created profile settings to `_Profiles`.
	 * 
	 * @note Uses UE_LOG for logging the start of the default settings loading process.
	 */
	void LoadDefaults() const;

	/**
	 * @brief Applies the specified graphics profile internally.
	 * 
	 * This function applies the given graphics profile by either reverting to the original user game settings or 
	 * applying the settings associated with the specified profile.
	 * 
	 * @param Profile The profile to apply. If the profile is `EProfile::NONE`, the function reverts to the original user game settings.
	 * @return bool Returns true if the profile was successfully applied; false otherwise.
	 * 
	 * @details 
	 * - If the `Profile` is `EProfile::NONE`:
	 *   - Logs a message indicating reversion to original user game settings.
	 *   - Retrieves the user game settings and applies them.
	 *   - Sets `_ActiveProfile` to `EProfile::NONE` and broadcasts the profile change.
	 *   - Returns true if successful, false otherwise.
	 * - Finds the profile name associated with the given `EProfile`.
	 * - Logs an error and returns false if the profile name is not found.
	 * - Logs a message indicating the profile being applied.
	 * - Finds the settings associated with the profile.
	 * - Logs a warning and returns false if the settings profile is not found.
	 * - Creates a render state context for applying settings.
	 * - Iterates through each setting in the profile and applies it using `ApplySetting`.
	 * - Sets `_ActiveProfile` to the specified profile and broadcasts the profile change.
	 * - Returns true indicating the profile was successfully applied.
	 * 
	 * @note Uses UE_LOG for logging messages, warnings, and errors. Uses `OnProfileChanged` to broadcast profile changes.
	 */
	bool ApplyProfileInternal(EProfile Profile);
	

	/**
	 * @brief Handles actions to be performed after the world initialization.
	 * 
	 * This function hooks into the world initialization process to set up the `UHarmonyLinkGraphics` settings. 
	 * It checks if the world is valid and is a game world, then initializes or reloads the settings as necessary and sets up a timer for periodic updates.
	 * 
	 * @param World The world that has just been initialized.
	 * @param IVS Initialization values for the world.
	 * 
	 * @details 
	 * - Checks if the `World` pointer is valid. Logs an error and returns if the world is invalid.
	 * - If the world is a game world:
	 *   - Retrieves the singleton settings instance using `GetSettings`.
	 *   - If the world is a play-in-editor world, reloads the settings configuration by calling `LoadConfig` with `bForceReload` set to true.
	 *   - Checks if the tick timer is already set or active. If not, sets a timer to call the `Tick` function at intervals specified by `_TickRate`.
	 * 
	 * @note Uses UE_LOG for logging errors and informational messages.
	 */
	static void OnPostWorldInitialization(UWorld* World, UWorld::InitializationValues IVS);

	/**
	 * @brief Handles actions to be performed when the world ends.
	 * 
	 * This function is called when the world is about to be destroyed. It clears the tick timer and safely destroys the singleton instance of the `UHarmonyLinkGraphics` settings.
	 * 
	 * @param World The world that is ending.
	 * 
	 * @details 
	 * - Checks if the `World` pointer is valid. Logs an error and returns if the world is already destroyed.
	 * - If the tick timer exists, clears the timer using the world's timer manager.
	 * - Calls `DestroySettings` to safely destroy the singleton instance of `UHarmonyLinkGraphics`.
	 * 
	 * @note Uses UE_LOG for logging errors and informational messages.
	 */
	static void OnWorldEnd(UWorld* World);

	/**
	 * @brief Resets the singleton instance of the UHarmonyLinkGraphics settings.
	 * 
	 * This function resets the singleton instance by destroying the current instance and creating a new one.
	 * 
	 * @details 
	 * - Calls `DestroySettings` on the current instance to safely destroy it.
	 * - Calls `GetSettings` to create and initialize a new singleton instance.
	 */
	static void ResetInstance();

	/**
	 * @brief Applies a specific configuration setting to the console variable.
	 * 
	 * This function applies the given configuration setting to the corresponding console variable (CVar) based on its key. It supports different data types such as bool, float, and int.
	 * 
	 * @param Setting The key-value pair representing the setting to apply. The key is the name of the console variable, and the value is the configuration value.
	 * 
	 * @details 
	 * - Uses the console manager to find the console variable associated with the given setting key.
	 * - Depending on the type of the configuration value, sets the console variable to the corresponding value.
	 * - If the console variable is not found, logs a warning message.
	 * - If the value type is unsupported, logs a warning message.
	 * 
	 * @note Uses UE_LOG for logging warnings about unsupported value types or missing console variables.
	 */
	static void ApplySetting(const TPair<FName, FHLConfigValue>& Setting);

	/**
	 * @brief Prints debug information for all profiles.
	 * 
	 * This function logs the debug information for all profiles stored in `_Profiles`. It iterates through each profile and calls `PrintDebugSection` to print the details.
	 * 
	 * @details 
	 * - Logs the start of the `DebugPrintProfiles` process.
	 * - Iterates over each profile in the `_Profiles` map.
	 * - Calls `PrintDebugSection` for each profile to print its details.
	 * - Logs the completion of the `DebugPrintProfiles` process.
	 * 
	 * @note Uses UE_LOG for logging the start and completion of the debug print process.
	 */
	void DebugPrintProfiles() const;

	FConfigFile* GetConfig() const;

	/**
	 * @brief Prints debug information for a specific settings profile.
	 * 
	 * This function logs the debug information for a given settings profile, including the section name and each setting's key, value, and type.
	 * 
	 * @param SettingsProfile The settings profile to print debug information for.
	 * 
	 * @details 
	 * - Logs the section name of the `SettingsProfile`.
	 * - Iterates through each setting in the `SettingsProfile`.
	 * - Depending on the type of each setting, converts the value to a string and logs the key, value, and type.
	 * 
	 * @note Uses UE_LOG for logging the section name and settings details.
	 */
	static void PrintDebugSection(FSettingsProfile& SettingsProfile);

	// Indicates whether automatic profile switching is enabled.
	static bool _bAutomaticSwitch;

	// Stores the last recorded battery percentage.
	static int32 _LastBatteryPercentage;

	// The rate at which to query HarmonyLinkLib for hardware info.
	static int32 _TickRate;

	// Timer handle for managing the periodic tick function.
	static FTimerHandle _TickTimerHandle;

	// The currently active profile.
	static EProfile _ActiveProfile;

	// Maps profile enums to their corresponding names.
	static TMap<EProfile, FName> _ProfileNames;

	// Stores the settings profiles.
	static TMap<EProfile, FSettingsProfile> _Profiles;

	static TSharedPtr<FConfigFile> _ConfigFile;

	// The default settings for profiles.
	static TMap<FName, TMap<FName, FHLConfigValue>> _DefaultSettings;

	// Singleton instance of UHarmonyLinkGraphics.
	static UHarmonyLinkGraphics* _INSTANCE;
};
