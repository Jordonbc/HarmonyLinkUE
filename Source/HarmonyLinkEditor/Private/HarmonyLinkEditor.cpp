#include "HarmonyLinkEditor.h"

#include "ISettingsModule.h"
#include "Objects/HarmonyLinkSettings.h"
#include "Objects/HarmonyLinkSettingsCustomization.h"

#define LOCTEXT_NAMESPACE "FHarmonyLinkEditorModule"

void FHarmonyLinkEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("HarmonyLinkSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FHarmonyLinkSettingsCustomization::MakeInstance));
	
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "HarmonyLink",
			LOCTEXT("HarmonyLinkSettingsName", "HarmonyLink Settings"),
			LOCTEXT("HarmonyLinkSettingsDescription", "Configure the HarmonyLink plugin settings."),
			GetMutableDefault<UHarmonyLinkSettings>()
		);
	}
}

void FHarmonyLinkEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout("HarmonyLinkSettings");
	}
	
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "HarmonyLink");
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHarmonyLinkEditorModule, HarmonyLinkEditor)
