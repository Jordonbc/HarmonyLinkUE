// Copyright (C) 2024 Jordon Brooks

#include "HarmonyLinkUE.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FHarmonyLinkUEModule"

DEFINE_LOG_CATEGORY(LogHarmonyLink);

void FHarmonyLinkUEModule::StartupModule()
{
	
}

void FHarmonyLinkUEModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHarmonyLinkUEModule, HarmonyLinkUE)
