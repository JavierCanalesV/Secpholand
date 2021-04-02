// Copyright 2020 Andrew Bindraw. All Rights Reserved.

#include "SystemMicManager.h"

FSystemMicManager *FSystemMicManager::Instance = nullptr;

FSystemMicManager::FSystemMicManager() : 
	DeviceTopology(nullptr),
	ConnectorFrom(nullptr),
	ConnectorTo(nullptr),
	AudioVolumeLevel(nullptr),
	AudioEndpointVolume(nullptr),
	DefaultDevice(nullptr),
	DeviceEnumerator(nullptr),
	DevicesCollection(nullptr),
	PropertyStore(nullptr),
	PolicyConfigVista(nullptr),
	PolicyConfig(nullptr)
{
	// PartIdForMicBoost, this id correct for IPart::GetLocalId of Microphone Boost on Windows 10.
	// Why use id? How find id? Please, check method FSystemMicManager::GetPartMicBoost, commented Experimental code.
	PartIdForMicBoost = 131073; 

	FWindowsPlatformMisc::CoInitialize();

	CoCreateInstance(__uuidof(CPolicyConfigVistaClient), nullptr, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&PolicyConfigVista);

	// For Win10
	HRESULT Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig2, (LPVOID *)&PolicyConfig);
	if (Result != S_OK)
	{
		Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig1, (LPVOID *)&PolicyConfig);
	}

	// For Win Vista, 7, 8, 8.1
	if (Result != S_OK)
	{
		Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig0, (LPVOID *)&PolicyConfig);
	}

	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&DeviceEnumerator);
}

FSystemMicManager *FSystemMicManager::Get()
{
	if(Instance == nullptr)
	{
		Instance = new FSystemMicManager;
	}
	
	return Instance;
}

FSystemMicManager::~FSystemMicManager()
{
	FWindowsPlatformMisc::CoUninitialize();
}

void FSystemMicManager::DestroyInstance()
{
	if(Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}


bool FSystemMicManager::SetDefaultDevice(const FString &DeviceId)
{
	if (GetDefaultDeviceId() == DeviceId)
	{
		return false;
	}

	if (!PolicyConfigVista)
	{
		return false;
	}

	PolicyConfigVista->SetDefaultEndpoint(reinterpret_cast<wchar_t *>(TCHAR_TO_UTF16(*DeviceId)), eConsole);
	PolicyConfigVista->SetDefaultEndpoint(reinterpret_cast<wchar_t *>(TCHAR_TO_UTF16(*DeviceId)), eMultimedia);
	PolicyConfigVista->SetDefaultEndpoint(reinterpret_cast<wchar_t *>(TCHAR_TO_UTF16(*DeviceId)), eCommunications);

	return true;
}

FString FSystemMicManager::GetDefaultDeviceName()
{
	return GetDeviceNameFromId(GetDefaultDeviceId());
}

FString FSystemMicManager::GetDefaultDeviceId()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return FString(TEXT(""));
	}

	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371405(v=vs.85).aspx, see Return value!
	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return FString(TEXT(""));
	}

	WCHAR* swDeviceId = reinterpret_cast<wchar_t *>(TCHAR_TO_UTF16(*FString(TEXT(""))));
	Result = DefaultDevice->GetId(&swDeviceId);
	if (Result != S_OK)
	{
		return FString(TEXT(""));
	}

	return FString(UTF16_TO_TCHAR(swDeviceId));
}

FString FSystemMicManager::GetDeviceIdFromName(const FString &DeviceName)
{
	FString DeviceId;

	for (TPair<FString, FString>& Device : GetActiveDevices())
	{
		if (Device.Value == DeviceName)
		{
			DeviceId = Device.Key;
		}
	}

	return DeviceId;
}

FString FSystemMicManager::GetDeviceNameFromId(const FString &DeviceId)
{
	FString DeviceName;

	for (TPair<FString, FString>& Device : GetActiveDevices())
	{
		if (Device.Key == DeviceId)
		{
			DeviceName = Device.Value;
		}
	}

	return DeviceName;
}

TMap<FString, FString> FSystemMicManager::GetActiveDevices()
{
	TMap<FString, FString> ActiveDevices;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371400(v=vs.85).aspx, see Return value!
	HRESULT Result = DeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &DevicesCollection);
	if (Result != S_OK)
	{
		return TMap<FString, FString>();
	}

	UINT CountActiveDevices = 0;
	DevicesCollection->GetCount(&CountActiveDevices);
	LPWSTR pwszID = nullptr;

	for (UINT i = 0; i < CountActiveDevices; i++)
	{
		DevicesCollection->Item(i, &DefaultDevice);
		DefaultDevice->GetId(&pwszID);
		DefaultDevice->OpenPropertyStore(STGM_READ, &PropertyStore);

		PROPVARIANT nameDevice;
		PropVariantInit(&nameDevice);
		PropertyStore->GetValue(PKEY_Device_FriendlyName, &nameDevice);

		ActiveDevices.Add(FString(UTF16_TO_TCHAR(pwszID)));
		ActiveDevices[FString(UTF16_TO_TCHAR(pwszID))] = FString(UTF16_TO_TCHAR(nameDevice.pwszVal));

		CoTaskMemFree(pwszID);
		pwszID = nullptr;
	}

	if (PropertyStore)
	{
		PropertyStore->Release();
		PropertyStore = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	if (DevicesCollection)
	{
		DevicesCollection->Release();
		DevicesCollection = nullptr;
	}

	return ActiveDevices;
}


void FSystemMicManager::SetVolume(float Value)
{
	float MicVolume = this->GetScalarFromValue(Value);
	
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return;
	}

	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return;
	}

	Result = AudioEndpointVolume->SetMasterVolumeLevelScalar(MicVolume, nullptr);
	if (Result != S_OK)
	{
		return;
	}

	if (AudioEndpointVolume)
	{
		AudioEndpointVolume->Release();
		AudioEndpointVolume = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}
}

float FSystemMicManager::GetVolume()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	float MicVolume = 0.0f;
	Result = AudioEndpointVolume->GetMasterVolumeLevelScalar(&MicVolume);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	if (AudioEndpointVolume)
	{
		AudioEndpointVolume->Release();
		AudioEndpointVolume = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	return GetValueFromScalar(MicVolume);
}


void FSystemMicManager::SetMute(bool IsMuted)
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return;
	}

	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return;
	}

	Result = AudioEndpointVolume->SetMute(IsMuted, nullptr);
	if (Result != S_OK)
	{
		return;
	}

	if (AudioEndpointVolume)
	{
		AudioEndpointVolume->Release();
		AudioEndpointVolume = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}
}

bool FSystemMicManager::GetMute()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return false;
	}

	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return false;
	}

	BOOL bIsMuted = 0;
	Result = AudioEndpointVolume->GetMute(&bIsMuted);
	if (Result != S_OK)
	{
		return false;
	}

	if (AudioEndpointVolume)
	{
		AudioEndpointVolume->Release();
		AudioEndpointVolume = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	return bIsMuted;
}


void FSystemMicManager::SetBoost(float Value)
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return;
	}

	Result = DefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&DeviceTopology);
	if (Result != S_OK)
	{
		return;
	}

	IPart *PartMicBoost = GetPartMicBoost(&DeviceTopology);
	if (!IsPartIdMicBoost(&PartMicBoost))
	{
		return;
	}

	Result = PartMicBoost->Activate(CLSCTX_ALL, __uuidof(IAudioVolumeLevel), (PVOID *)&AudioVolumeLevel);
	if (Result != S_OK)
	{
		return;
	}

	UINT CountChannels = 0;
	Result = AudioVolumeLevel->GetChannelCount(&CountChannels);
	if (Result != S_OK)
	{
		return;
	}

	for (UINT i = 0; i < CountChannels; ++i)
	{
		AudioVolumeLevel->SetLevel(i, Value, nullptr);
	}

	if (AudioVolumeLevel)
	{
		AudioVolumeLevel->Release();
		AudioVolumeLevel = nullptr;
	}

	if (PartMicBoost)
	{
		PartMicBoost->Release();
		PartMicBoost = nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}

	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	if (DeviceTopology)
	{
		DeviceTopology->Release();
		DeviceTopology = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}
}

float FSystemMicManager::GetBoost()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = DefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&DeviceTopology);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	IPart *PartMicBoost = GetPartMicBoost(&DeviceTopology);
	if (!IsPartIdMicBoost(&PartMicBoost))
	{
		return 0.0f;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Try init IAudioVolumeLevel for MicBoost..."));
	Result = PartMicBoost->Activate(CLSCTX_ALL, __uuidof(IAudioVolumeLevel), (PVOID *)&AudioVolumeLevel);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Try init AudioVolumeLevel->GetChannelCount for MicBoost..."));
	UINT CountChannels = 0;
	Result = AudioVolumeLevel->GetChannelCount(&CountChannels);
	if (Result != S_OK)
	{
		return 0.0f;
	}
	//UE_LOG(LogTemp, Warning, TEXT("CountChannels: %d"), CountChannels);

	float VolumeDb = 0.0f;
	for (UINT i = 0; i < CountChannels; ++i)
	{
		AudioVolumeLevel->GetLevel(i, &VolumeDb);
	}

	if (AudioVolumeLevel)
	{
		AudioVolumeLevel->Release();
		AudioVolumeLevel = nullptr;
	}

	if (PartMicBoost)
	{
		PartMicBoost->Release();
		PartMicBoost = nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}
	
	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	if (DeviceTopology)
	{
		DeviceTopology->Release();
		DeviceTopology = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	return VolumeDb;
}

float FSystemMicManager::GetBoostMin()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = DefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&DeviceTopology);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	IPart *PartMicBoost = GetPartMicBoost(&DeviceTopology);
	if (!IsPartIdMicBoost(&PartMicBoost))
	{
		return 0.0f;
	}

	float MinLevelDb = 0.0f, MaxLevelDb = 0.0f, StepLevel = 0.0f;
	GetBoostLevelRange(&PartMicBoost, &MinLevelDb, &MaxLevelDb, &StepLevel);

	if (PartMicBoost)
	{
		PartMicBoost->Release();
		PartMicBoost = nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}

	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	if (DeviceTopology)
	{
		DeviceTopology->Release();
		DeviceTopology = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	return MinLevelDb;
}

float FSystemMicManager::GetBoostMax()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = DefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&DeviceTopology);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	IPart *PartMicBoost = GetPartMicBoost(&DeviceTopology);
	if (!IsPartIdMicBoost(&PartMicBoost))
	{
		return 0.0f;
	}

	float MinLevelDb = 0.0f, MaxLevelDb = 0.0f, StepLevel = 0.0f;
	GetBoostLevelRange(&PartMicBoost, &MinLevelDb, &MaxLevelDb, &StepLevel);

	if (PartMicBoost)
	{
		PartMicBoost->Release();
		PartMicBoost = nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}

	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	if (DeviceTopology)
	{
		DeviceTopology->Release();
		DeviceTopology = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	return MaxLevelDb;
}

float FSystemMicManager::GetBoostStep()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = DefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&DeviceTopology);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	IPart *PartMicBoost = GetPartMicBoost(&DeviceTopology);
	if (!IsPartIdMicBoost(&PartMicBoost))
	{
		return 0.0f;
	}

	float MinLevelDb = 0.0f, MaxLevelDb = 0.0f, StepLevel = 0.0f;
	GetBoostLevelRange(&PartMicBoost, &MinLevelDb, &MaxLevelDb, &StepLevel);

	if (PartMicBoost)
	{
		PartMicBoost->Release();
		PartMicBoost = nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}

	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	if (DeviceTopology)
	{
		DeviceTopology->Release();
		DeviceTopology = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	return StepLevel;
}

bool FSystemMicManager::IsBoostSupported()
{
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return false;
	}

	Result = DefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID*)&DeviceTopology);
	if (Result != S_OK)
	{
		return false;
	}

	IPart *PartMicBoost = GetPartMicBoost(&DeviceTopology);
	return IsPartIdMicBoost(&PartMicBoost);
}

void FSystemMicManager::GetBoostLevelRange(IPart **PartMicBoost, float *OutMinLevelDb, float *OutMaxLevelDb, float *OutStepLevel)
{
	if (*PartMicBoost == nullptr)
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Try init IAudioVolumeLevel for MicBoost..."));
	HRESULT Result = (*PartMicBoost)->Activate(CLSCTX_ALL, __uuidof(IAudioVolumeLevel), (PVOID*)&AudioVolumeLevel);
	if (Result != S_OK)
	{
		return;
	}

	UINT CountChannels = 0;
	Result = AudioVolumeLevel->GetChannelCount(&CountChannels);
	if (Result != S_OK)
	{
		return;
	}

	float MinLevelDb = 0.0f, MaxLevelDb = 0.0f, StepLevel = 0.0f;
	for (UINT i = 0; i < CountChannels; ++i)
	{
		AudioVolumeLevel->GetLevelRange(i, &MinLevelDb, &MaxLevelDb, &StepLevel);
	}

	*OutMinLevelDb = MinLevelDb;
	*OutMaxLevelDb = MaxLevelDb;
	*OutStepLevel = StepLevel;

	if (AudioVolumeLevel)
	{
		AudioVolumeLevel->Release();
		AudioVolumeLevel = nullptr;
	}
}

IPart *FSystemMicManager::GetPartMicBoost(IDeviceTopology **OutDeviceTopology)
{
	if (*OutDeviceTopology == nullptr)
	{
		return nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}

	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	HRESULT Result = (*OutDeviceTopology)->GetConnector(0, &ConnectorFrom);
	if (Result != S_OK)
	{
		return nullptr;
	}

	Result = ConnectorFrom->GetConnectedTo(&ConnectorTo);
	if (Result != S_OK)
	{
		return nullptr;
	}

	IPart *PartItem = nullptr;
	Result = ConnectorTo->QueryInterface(__uuidof(IPart), (PVOID *)&PartItem);
	if (Result != S_OK)
	{
		return nullptr;
	}

	if (*OutDeviceTopology)
	{
		(*OutDeviceTopology)->Release();
		(*OutDeviceTopology) = nullptr;
	}

	// Find IPart from DeviceTopology by PartIdForMicBoost
	IDeviceTopology *DeviceTopologyItem = nullptr;
	Result = PartItem->GetTopologyObject(&DeviceTopologyItem);
	if (Result != S_OK)
	{
		return nullptr;
	}

	IPart *PartMicBoost = nullptr;
	Result = DeviceTopologyItem->GetPartById(PartIdForMicBoost, &PartMicBoost);
	if (Result != S_OK)
	{
		return GetPartMicBoost(&DeviceTopologyItem);
	}

	return PartMicBoost;


	/*
	// =============================================================
	// Experimental code, find id for Microphone Boost
	// =============================================================

	if (*OutDeviceTopology == nullptr)
	{
		return nullptr;
	}

	if (ConnectorTo)
	{
		ConnectorTo->Release();
		ConnectorTo = nullptr;
	}

	if (ConnectorFrom)
	{
		ConnectorFrom->Release();
		ConnectorFrom = nullptr;
	}

	UINT CountSubUnit;
	HRESULT Result = (*OutDeviceTopology)->GetSubunitCount(&CountSubUnit);
	if (Result != S_OK)
	{
		return nullptr;
	}
	//UE_LOG(LogTemp, Warning, TEXT("(*OutDeviceTopology)->GetSubunitCount: %d"), CountSubUnit);

	UINT CountConnector;
	Result = (*OutDeviceTopology)->GetConnectorCount(&CountConnector);
	if (Result != S_OK)
	{
		return nullptr;
	}
	//UE_LOG(LogTemp, Warning, TEXT("(*OutDeviceTopology)->GetConnectorCount: %d"), CountConnector);

	Result = (*OutDeviceTopology)->GetConnector(0, &ConnectorFrom);
	if (Result != S_OK)
	{
		return nullptr;
	}

	Result = ConnectorFrom->GetConnectedTo(&ConnectorTo);
	if (Result != S_OK)
	{
		return nullptr;
	}

	IPart *PartItem = nullptr;
	Result = ConnectorTo->QueryInterface(__uuidof(IPart), (PVOID *)&PartItem);
	if (Result != S_OK)
	{
		return nullptr;
	}

	if (CountSubUnit > 0)
	{
		for (UINT i = 0; i < CountSubUnit; ++i)
		{
			ISubunit *SubUnit = nullptr;
			Result = (*OutDeviceTopology)->GetSubunit(i, &SubUnit);
			if (Result != S_OK)
			{
				return nullptr;
			}

			Result = SubUnit->QueryInterface(__uuidof(IPart), (PVOID *)&PartItem);
			if (Result != S_OK)
			{
				return nullptr;
			}

			// ===============================================================
			// WARNING! On some Win10 systems possible crash in ucrtbase.dll
			// https://superuser.com/questions/1397010/application-crash-due-to-ucrtbase-dll
			// Unsolved at the moment but crashed inexplicably at trying to use a _COM_SMARTPTR_TYPEDEF which worked previously in Win7 but not in Win10.
			// ===============================================================
			LPWSTR PartName = nullptr;
			PartItem->GetName(&PartName);
			const FString PartNameStr = FString(UTF16_TO_TCHAR(PartName));

			LPWSTR PartGlobalId = nullptr;
			PartItem->GetGlobalId(&PartGlobalId);
			const FString PartGlobalIdStr = FString(UTF16_TO_TCHAR(PartGlobalId));
			
			UINT PartLocalId;
			PartItem->GetLocalId(&PartLocalId);
			
			UINT CountControllers;
			PartItem->GetControlInterfaceCount(&CountControllers);
			
			GUID guid;
			Result = PartItem->GetSubType(&guid);
			if (Result != S_OK)
			{
				return nullptr;
			}
			
			OLECHAR wsguid[256];
			int length = StringFromGUID2(guid, wsguid, 256);
			if (length == 0)
			{
				return nullptr;
			}
			const FString GuidStr = FString(UTF16_TO_TCHAR(wsguid));
			
			// <ksmedia.h> GUID: 3A5ACC00-C557-11D0-8A2B-00A0C9255AC1, KSNODETYPE_VOLUME, PartLocalId: 131073
			UE_LOG(LogTemp, Warning, TEXT("i: %d, PartName: %s, PartLocalId: %d, CountControllers: %d, GUID: %s, PartGlobalId: %s"),
				i, PartNameStr.IsEmpty() ? TEXT("NoName") : *PartNameStr,
				PartLocalId,
				CountControllers,
				*GuidStr,
				*PartGlobalIdStr);
			
			CoTaskMemFree(PartGlobalId);
			PartGlobalId = nullptr;

			CoTaskMemFree(PartName);
			PartName = nullptr;

			if (SubUnit)
			{
				SubUnit->Release();
				SubUnit = nullptr;
			}

			// FIXME: How detect on system with different language? Maybe GUID: 2BC31D6A-96E3-11d2-AC4C-00C04F8EFB68, KSAUDFNAME_MICROPHONE_BOOST ?
			if (PartNameStr.Contains((TEXT("Microphone Boost"))))
			{
				return PartItem;
			}
		}
	}

	if (*OutDeviceTopology)
	{
		(*OutDeviceTopology)->Release();
		(*OutDeviceTopology) = nullptr;
	}

	IDeviceTopology *DeviceTopologyItem = nullptr;
	Result = PartItem->GetTopologyObject(&DeviceTopologyItem);
	if (Result != S_OK)
	{
		return nullptr;
	}

	return GetPartMicBoost(&DeviceTopologyItem);
	*/
}




// ============================
// Private tools, FORCEINLINE
// ============================
bool FSystemMicManager::IsPartIdMicBoost(IPart **PartMicBoost)
{
	if (*PartMicBoost == nullptr)
	{
		return false;
	}

	UINT nPartId = 0;
	HRESULT Result = (*PartMicBoost)->GetLocalId(&nPartId);
	if (Result != S_OK)
	{
		return false;
	}

	return nPartId == PartIdForMicBoost;
}

float FSystemMicManager::GetScalarFromValue(int32 Value)
{
	return FMath::Abs(Value) >= 100.0f ? 1.0f : Value / 100.0f;
}

float FSystemMicManager::GetValueFromScalar(float Value)
{
	return FMath::RoundToFloat(FMath::Abs(Value) > 0.0f ? Value * 100.0f : 0.0f);
}