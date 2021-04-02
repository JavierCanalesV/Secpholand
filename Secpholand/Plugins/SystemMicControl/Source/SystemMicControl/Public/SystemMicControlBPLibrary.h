// Copyright 2020 Andrew Bindraw. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SystemMicManager.h"
#include "SystemMicControlBPLibrary.generated.h"

UCLASS()
class USystemMicControlBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "SystemMicControl", DisplayName = "Set System Mic Default Device")
	static bool SetMicDefaultDevice(const FString &DeviceId);

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Default Device Name")
	static FString GetMicDefaultDeviceName();

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Default Device Id")
	static FString GetMicDefaultDeviceId();

	UFUNCTION(BlueprintCallable, Category = "SystemMicControl", DisplayName = "Get System Mic Device Id From Name")
	static void GetMicDeviceIdFromName(const FString &InDeviceName, FString &OutDeviceId);

	UFUNCTION(BlueprintCallable, Category = "SystemMicControl", DisplayName = "Get System Mic Device Name From Id")
	static void GetMicDeviceNameFromId(const FString &InDeviceId, FString &OutDeviceName);

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Active Devices")
	static TMap<FString, FString> GetMicActiveDevices();


	UFUNCTION(BlueprintCallable, Category = "SystemMicControl", DisplayName = "Set System Mic Volume")
	static void SetMicVolume(float Value);

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Volume")
	static float GetMicVolume();
	
	
	UFUNCTION(BlueprintCallable, Category = "SystemMicControl", DisplayName = "Set System Mic Boost")
	static void SetMicBoost(float Value);

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Boost")
	static float GetMicBoost();

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Boost Min")
	static float GetMicBoostMin();

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Boost Max")
	static float GetMicBoostMax();

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Boost Step")
	static float GetMicBoostStep();

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Is System Mic Boost Supported")
	static bool IsMicBoostSupported();


	UFUNCTION(BlueprintCallable, Category = "SystemMicControl", DisplayName = "Set System Mic Mute")
	static void SetMicMute(bool IsMuted);

	UFUNCTION(BlueprintPure, Category = "SystemMicControl", DisplayName = "Get System Mic Mute")
	static bool GetMicMute();
};
