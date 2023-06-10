// Copyright Jordon Brooks © 2023

#pragma once

#include "Modules/ModuleManager.h"

class FHarmonyLinkUEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
