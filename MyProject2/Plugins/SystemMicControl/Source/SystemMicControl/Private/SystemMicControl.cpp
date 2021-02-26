// Copyright 2020 Andrew Bindraw. All Rights Reserved.

#include "SystemMicControl.h"
#include "SystemMicManager.h"

#define LOCTEXT_NAMESPACE "FSystemMicControlModule"

void FSystemMicControlModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSystemMicManager::Get();
}

void FSystemMicControlModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FSystemMicManager::Get()->DestroyInstance();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSystemMicControlModule, SystemMicControl)