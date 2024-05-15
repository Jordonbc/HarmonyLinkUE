// Copyright (C) 2024 Jordon Brooks

#include "HarmonyLink.h"
#include "Modules/ModuleManager.h"
#include "Objects/HarmonyLinkGraphics.h"

#define LOCTEXT_NAMESPACE "FHarmonyLinkModule"

DEFINE_LOG_CATEGORY(LogHarmonyLink);

void FHarmonyLinkModule::StartupModule()
{
	UHarmonyLinkGraphics::GetSettings();
}

void FHarmonyLinkModule::ShutdownModule()
{
	UHarmonyLinkGraphics::DestroySettings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHarmonyLinkModule, HarmonyLink)
