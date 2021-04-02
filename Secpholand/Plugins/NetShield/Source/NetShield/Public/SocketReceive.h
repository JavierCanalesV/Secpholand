// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Async/AsyncWork.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "Socket.h"

#include "SocketReceive.generated.h"

// Blueprint Callback for OnSuccess/OnFail
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketReceiveOutput, FString, Message);
// Internal Callback for ASync Task Callback
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSocketReceiveHandler, bool, Success, FString, buffer);

// The async socket receive task
class FSocketReceiveTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FSocketReceiveTask>;

public:
	FSocketReceiveTask(FSocketReceiveHandler Input, USocket* _Socket) :
		MyInput(Input),
		Socket(_Socket)
	{}

protected:
	FSocketReceiveHandler MyInput;
	USocket *Socket;
	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSocketReceiveTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

/**
 * 
 */

 // Class Used For Async TCP Socket Recieving Using Blueprints. Calls Back The OnSuccess/OnFail Delegate When A Packet Has Been Recieved
UCLASS()
class NETSHIELD_API USocketReceive : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

// Public Functions
public:
	// Secure And Insecure TCP Receive
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "TCP Receive", Keywords = "NetShield socket receive TCP TCPreceive", BlueprintInternalUseOnly = "true"), Category = "NetShield|Sockets")
		static USocketReceive* CallbackLater(USocket *Socket);

	// Callback Delegates
public:
	UPROPERTY(BlueprintAssignable)
		FSocketReceiveOutput OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FSocketReceiveOutput OnFail;

	// Internal functions/delegates
private:
	FSocketReceiveHandler myDelegate;
	void Start(USocket *Socket);
	UFUNCTION()
		void HandleCallback(bool Success, FString Message);
	
};
