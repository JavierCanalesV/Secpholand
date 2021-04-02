// Copyright 2020 Andrew Bindraw. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSystemMicControlModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
