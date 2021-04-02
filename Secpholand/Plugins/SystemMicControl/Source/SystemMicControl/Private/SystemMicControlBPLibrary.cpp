// Copyright 2020 Andrew Bindraw. All Rights Reserved.

#include "SystemMicControlBPLibrary.h"
#include "SystemMicControl.h"

USystemMicControlBPLibrary::USystemMicControlBPLibrary(const FObjectInitializer &ObjectInitializer)
: Super(ObjectInitializer)
{
	
}


bool USystemMicControlBPLibrary::SetMicDefaultDevice(const FString &DeviceId)
{
	return FSystemMicManager::Get()->SetDefaultDevice(DeviceId);
}

FString USystemMicControlBPLibrary::GetMicDefaultDeviceName()
{
	return FSystemMicManager::Get()->GetDefaultDeviceName();
}

FString USystemMicControlBPLibrary::GetMicDefaultDeviceId()
{
	return FSystemMicManager::Get()->GetDefaultDeviceId();
}

void USystemMicControlBPLibrary::GetMicDeviceIdFromName(const FString &InDeviceName, FString &OutDeviceId)
{
	OutDeviceId = FSystemMicManager::Get()->GetDeviceIdFromName(InDeviceName);
}

void USystemMicControlBPLibrary::GetMicDeviceNameFromId(const FString &InDeviceId, FString &OutDeviceName)
{
	OutDeviceName = FSystemMicManager::Get()->GetDeviceNameFromId(InDeviceId);
}

TMap<FString, FString> USystemMicControlBPLibrary::GetMicActiveDevices()
{
	return FSystemMicManager::Get()->GetActiveDevices();
}


void USystemMicControlBPLibrary::SetMicVolume(float Value)
{
	FSystemMicManager::Get()->SetVolume(Value);
}

float USystemMicControlBPLibrary::GetMicVolume()
{
	return FSystemMicManager::Get()->GetVolume();
}


void USystemMicControlBPLibrary::SetMicBoost(float Value)
{
	FSystemMicManager::Get()->SetBoost(Value);
}

float USystemMicControlBPLibrary::GetMicBoost()
{
	return FSystemMicManager::Get()->GetBoost();
}

float USystemMicControlBPLibrary::GetMicBoostMin()
{
	return FSystemMicManager::Get()->GetBoostMin();
}

float USystemMicControlBPLibrary::GetMicBoostMax()
{
	return FSystemMicManager::Get()->GetBoostMax();
}

float USystemMicControlBPLibrary::GetMicBoostStep()
{
	return FSystemMicManager::Get()->GetBoostStep();
}

bool USystemMicControlBPLibrary::IsMicBoostSupported()
{
	return FSystemMicManager::Get()->IsBoostSupported();
}


void USystemMicControlBPLibrary::SetMicMute(bool IsMuted)
{
	FSystemMicManager::Get()->SetMute(IsMuted);
}

bool USystemMicControlBPLibrary::GetMicMute()
{
	return FSystemMicManager::Get()->GetMute();
}
