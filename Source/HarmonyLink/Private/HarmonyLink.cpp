// Copyright (C) 2024 Jordon Brooks

#include "HarmonyLink.h"
#include "Modules/ModuleManager.h"
#include "Objects/HarmonyLinkGraphics.h"

#define LOCTEXT_NAMESPACE "FHarmonyLinkModule"

DEFINE_LOG_CATEGORY(LogHarmonyLink);

void FHarmonyLinkModule::StartupModule()
{
	
}

void FHarmonyLinkModule::ShutdownModule()
{
	// Ensure we safely destroy our singleton instance
	UHarmonyLinkGraphics::DestroySettings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHarmonyLinkModule, HarmonyLink)
