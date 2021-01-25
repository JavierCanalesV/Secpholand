// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Async/AsyncWork.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "Socket.h"

#include "SocketClose.generated.h"


// OnSuccess / OnFail Blueprint Callback Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSocketCloseOutput);
// Internal Close Handler Callback Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketCloseHandler, bool, Success);

// An ASync Task That Closes A Socket Connection
class FSocketCloseTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FSocketCloseTask>;

public:
	FSocketCloseTask(FSocketCloseHandler Input, USocket* _Socket) :
		MyInput(Input),
		Socket(_Socket)
	{}

protected:
	FSocketCloseHandler MyInput;
	USocket *Socket;
	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSocketCloseTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

/**
 * 
 */

 // Class Used For Async TCP Socket Closing Using Blueprints.
UCLASS()
class NETSHIELD_API USocketClose : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
// Public Blueprint-Callable Functions
public:
	// Secure And Insecure TCP Close
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "TCP Close", Keywords = "OpenSSL socket close TCP TCPclose NetShield", BlueprintInternalUseOnly = "true"), Category = "NetShield|Sockets")
		static USocketClose* CallbackLater(USocket *Socket);

	// Public Blueprint Callbacks
public:
	// Success Blueprint Callback
	UPROPERTY(BlueprintAssignable)
		FSocketCloseOutput OnSuccess;
	// Fail/Error Blueprint Callback
	UPROPERTY(BlueprintAssignable)
		FSocketCloseOutput OnFail;

	// Internal Functions/Delegates
private:
	// ASync Callback Delegate
	FSocketCloseHandler myDelegate;
	// Start Task Fucntion
	void Start(USocket *Socket);
	// ASync Task Callback Handler
	UFUNCTION()
		void HandleCallback(bool Success);
	
	
};
