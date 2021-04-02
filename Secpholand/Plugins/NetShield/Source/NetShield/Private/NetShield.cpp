// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "NetShield.h"

DEFINE_LOG_CATEGORY(LogNetShield);

#define LOCTEXT_NAMESPACE "FNetShieldModule"

void FNetShieldModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FNetShieldModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNetShieldModule, NetShield)