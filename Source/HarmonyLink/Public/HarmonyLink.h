// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "Modules/ModuleManager.h"

class FHarmonyLinkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
