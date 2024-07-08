// Copyright (C) 2024 Jordon Brooks

#include "HarmonyLinkSettings.h"

#include "Objects/HarmonyLinkGraphics.h"

#define LOCTEXT_NAMESPACE "FHarmonyLinkSettingsModule"

DEFINE_LOG_CATEGORY(LogHarmonyLinkSettings);

void FHarmonyLinkSettingsModule::StartupModule()
{
    
}

void FHarmonyLinkSettingsModule::ShutdownModule()
{
	// Ensure we safely destroy our singleton instance
	UHarmonyLinkGraphics::DestroySettings();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHarmonyLinkSettingsModule, HarmonyLinkSettings)
