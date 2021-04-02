// Copyright 2020 Andrew Bindraw. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmeapi.h>
#include <ksmedia.h>
#include "PolicyConfig.h"
#include "Windows/HideWindowsPlatformTypes.h"

class FSystemMicManager
{
	private:
		FSystemMicManager();
		~FSystemMicManager();
		
		static FSystemMicManager *Instance;

		IDeviceTopology			*DeviceTopology;
		IConnector				*ConnectorFrom;
		IConnector				*ConnectorTo;
		
		IAudioVolumeLevel       *AudioVolumeLevel;
		IAudioEndpointVolume    *AudioEndpointVolume;
		IMMDevice               *DefaultDevice;
		IMMDeviceEnumerator     *DeviceEnumerator;
		IMMDeviceCollection     *DevicesCollection;
		IPropertyStore          *PropertyStore;

		IPolicyConfigVista      *PolicyConfigVista;
		IPolicyConfig           *PolicyConfig;

	public:
		static FSystemMicManager *Get();
		
		static void DestroyInstance();
		

		bool SetDefaultDevice(const FString &DeviceId);

		FString GetDefaultDeviceName();

		FString GetDefaultDeviceId();

		FString GetDeviceIdFromName(const FString &DeviceName);

		FString GetDeviceNameFromId(const FString &DeviceId);

		TMap<FString, FString> GetActiveDevices();


		void SetBoost(float Value);

		float GetBoost();

		float GetBoostMin();

		float GetBoostMax();

		float GetBoostStep();

		bool IsBoostSupported();


		void SetVolume(float Value);

		float GetVolume();

		void SetMute(bool IsMuted);

		bool GetMute();

	private:
		UINT PartIdForMicBoost;

		FORCEINLINE bool IsPartIdMicBoost(IPart **PartMicBoost);

		FORCEINLINE void GetBoostLevelRange(IPart **Part, float *OutMinLevelDb, float *OutMaxLevelDb, float *OutStepLevel);

		FORCEINLINE IPart *GetPartMicBoost(IDeviceTopology **OutDeviceTopology);

		FORCEINLINE float GetScalarFromValue(int32 Value);

		FORCEINLINE float GetValueFromScalar(float Value);
};


