// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "NetShield.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Async/AsyncWork.h"
#include "Kismet/BlueprintAsyncActionBase.h"

// Includes for in-editor PIE termination detection
#if WITH_EDITOR
#include "Editor.h"
#endif

#include "Socket.h"

#include "ManagedSocketTask.generated.h"

// Enum to store the type of socket close
UENUM(BlueprintType)
enum class ESocketCloseReason : uint8
{
	SCR_Requested 	UMETA(DisplayName = "User-requested socket close"),
	SCR_Disconnected 	UMETA(DisplayName = "Disconnected by server"),
	SCR_UnknownError		UMETA(Displayname = "Unknown Error")
};

// Blueprint Callback (OnSuccess/OnFail) Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMSocketConnectOutput, UManagedSocket*, Socket);
// Internal Connect Handler Callback Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBlueprintCSCallback, bool, Success, UManagedSocket*, Socket);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMReceiveBytesHandler, const TArray<uint8>&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMReceiveMessageHandler, FString, Message);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMSocketClosedHandler, ESocketCloseReason, Reason);


UCLASS(BlueprintType)
class NETSHIELD_API UManagedSocket : public USocket
{
	GENERATED_BODY()
protected:
	UManagedSocket()
	{
		// Editor-only functionality that will close the socket upon PIE termination (BeginPlay won't get called until the editor shuts down)
#if WITH_EDITOR
		EndPIEDelegate = FEditorDelegates::EndPIE.AddUObject(this, &UManagedSocket::HandleExitPIE);
#endif
	}
public:
	/*
	* Handler event that gets invoked when a buffer of bytes is available.
	*/
	UPROPERTY(BlueprintAssignable, Category = "NetShield|Managed")
	FMReceiveBytesHandler OnReceiveBytes;

	/*
	* Handler event that gets invoked when a string buffer is available. NOTE: Can be called in conjunction with OnReceiveBytes, and string may contain invalid characters if your protocol doesn't send UTF8 data!
	*/
	UPROPERTY(BlueprintAssignable, Category = "NetShield|Managed")
	FMReceiveMessageHandler OnReceiveMessage;

public:
	// Override the BeginDestroy method so we can close the socket connection when the object is being destroyed due to it no longer being needed/being unloaded/the game or editor being closed/etc.
	virtual void BeginDestroy() override;

#if WITH_EDITOR
private:
	void HandleExitPIE(bool someBool);
#endif

public:
	// OUT buffer
	bool bShouldFlush;
	TArray<uint8_t> buffer;

private:
#if WITH_EDITOR
	FDelegateHandle EndPIEDelegate;
#endif

public:
	class FManagedSocketConnectionTask *Manager;
};

// ASync Socket Connect Task
class FManagedSocketConnectionTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FManagedSocketConnectionTask>;

public:
	FManagedSocketConnectionTask(FBlueprintCSCallback Input, FString _IP, int32 _Port, bool _Secure) :
		MyInput(Input),
		IP(_IP),
		Port(_Port),
		Secure(_Secure)
	{
		// TODO: Implement closing that toggles this variable!
		bIsSocketActive = true;
	}

public:
	bool bIsSocketActive;

protected:
	FBlueprintCSCallback MyInput;
	FString IP;
	int32 Port;
	bool Secure;
	void DoWork();

	UManagedSocket *mySocket;

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FManagedSocketConnectionTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

/**
* Task for managing a managed (SSL) socket's task. Will invoke the correct events when required, and allow for easy usage of sockets.
*/
UCLASS()
class NETSHIELD_API UManagedSocketConnect : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

		// Public Blueprint Functions
public:
	// Secure And Insecure TCP Connect
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Managed TCP Connect", Keywords = "OpenSSL socket connect TCP TCPconnect", BlueprintInternalUseOnly = "true"), Category = "NetShield|Managed")
	static UManagedSocketConnect* CallbackLater(FString IP, int32 Port, bool Secure);

	// Blueprint Callback Delegates
public:
	// Success Callback
	UPROPERTY(BlueprintAssignable)
		FMSocketConnectOutput OnSuccess;

	// Error/Failure Callback
	UPROPERTY(BlueprintAssignable)
		FMSocketConnectOutput OnFail;

	// Private variables/functions
private:
	FBlueprintCSCallback callbackDelegate;
	void Start(FString IP, int32 Port, bool Secure);

	UFUNCTION()
	void HandleCallback(bool Success, UManagedSocket* Socket);
};