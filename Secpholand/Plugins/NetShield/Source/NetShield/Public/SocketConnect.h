// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Async/AsyncWork.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "Socket.h"

#include "SocketConnect.generated.h"

// Blueprint Callback (OnSuccess/OnFail) Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketConnectOutput, USocket*, Socket);
// Internal Connect Handler Callback Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FThingHandler, bool, Success, USocket*, Socket);

// ASync Socket Connect Task
class FMyTaskName : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FMyTaskName>;

public:
	FMyTaskName(FThingHandler Input, FString _IP, int32 _Port, bool _Secure) :
		MyInput(Input),
		IP(_IP),
		Port(_Port),
		Secure(_Secure)
	{}

protected:
	FThingHandler MyInput;
	FString IP;
	int32 Port;
	bool Secure;
	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};

/**
 * 
 */

 // Class Used For Async (TLS) TCP Socket Connecting Using Blueprints. Calls Back The OnSuccess Delegate If A Connection Was Successfully Established.
UCLASS()
class NETSHIELD_API USocketConnect : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
		// Public Blueprint Functions
public:
	// Secure And Insecure TCP Connect
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "TCP Connect", Keywords = "OpenSSL socket connect TCP TCPconnect NetShield", BlueprintInternalUseOnly = "true"), Category = "NetShield|Sockets")
		static USocketConnect* CallbackLater(FString IP, int32 Port, bool Secure);

	// Blueprint Callback Delegates
public:
	// Success Callback
	UPROPERTY(BlueprintAssignable)
		FSocketConnectOutput OnSuccess;

	// Error/Failure Callback
	UPROPERTY(BlueprintAssignable)
		FSocketConnectOutput OnFail;

	// Private variables/functions
private:
	FThingHandler myDelegate;
	void Start(FString IP, int32 Port, bool Secure);

	UFUNCTION()
		void HandleCallback(bool Success, USocket* Socket);
};
