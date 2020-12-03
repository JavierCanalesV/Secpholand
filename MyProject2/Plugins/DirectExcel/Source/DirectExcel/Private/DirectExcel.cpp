// Copyright 2018 Jianzhao Fu. All Rights Reserved.

#include "DirectExcel.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FDirectExcelModule"

void FDirectExcelModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
//	FString BaseDir = IPluginManager::Get().FindPlugin("DirectExcel")->GetBaseDir();
//
//	// Add on the relative location of the third party dll and load it
//	FString LibraryPath;
//#if PLATFORM_WINDOWS
//	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/UE4Editor-XlntLib.dll"));
//#elif PLATFORM_MAC
//	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Mac/UE4Editor-XlntLib.dylib"));
//#elif PLATFORM_LINUX
//    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Linux/UE4Editor-XlntLib.so"));
//#elif PLATFORM_ANDROID
//	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Android/UE4Editor-XlntLib.so"));
//#elif PLATFORM_IOS
//	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Linux/UE4Editor-XlntLib.dylib"));
//#endif // PLATFORM_WINDOWS
//
//	mLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
//
//	if (mLibraryHandle)
//	{
//		// Call the test function in the third party library that opens a message box
//		//ExampleLibraryFunction();
//	}
//	else
//	{
//		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
//	}
}

void FDirectExcelModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//Free the dll handle
	/*FPlatformProcess::FreeDllHandle(mLibraryHandle);
	mLibraryHandle = nullptr;*/
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDirectExcelModule, DirectExcel)