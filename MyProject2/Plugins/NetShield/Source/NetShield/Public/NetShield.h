// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogNetShield, Log, All);

class FNetShieldModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};