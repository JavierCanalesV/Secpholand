// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Async/AsyncWork.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "Socket.h"

#include "SocketSend.generated.h"

// Blueprint Callback (OnSuccess/OnFail) Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSocketSendOutput);
// Internal callback delegate used for when the async task is complete
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketSendHandler, bool, Success);

// An ASync Task To Send A Packet To A Socket Connection
class FSocketSendTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FSocketSendTask>;

public:
	FSocketSendTask(FSocketSendHandler Input, USocket* _Socket, FString _Message) :
		MyInput(Input),
		Socket(_Socket),
		Message(_Message)
	{}

protected:
	// Callback Delegate
	FSocketSendHandler MyInput;
	// Socket that it needs to send the msg to
	USocket *Socket;
	// Text it needs to send
	FString Message;

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSocketSendTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

/**
 * 
 */

// Class Used For Async TCP Socket Sending Using Blueprints. Calls Back The OnSuccess Delegate If Everything Went OK Or The OnFail Delegate If Something Went Wrong
UCLASS()
class NETSHIELD_API USocketSend : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

// Public Blueprint-Callable Functions
public:
	// Secure And Insecure TCP Send
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "TCP Send", Keywords = "OpenSSL socket send TCP TCPsend NetShield", BlueprintInternalUseOnly = "true"), Category = "NetShield|Sockets")
		static USocketSend* CallbackLater(USocket *Socket, FString Message);

	// Blueprint Callbacks
public:
	// Success Blueprint Callback
	UPROPERTY(BlueprintAssignable)
		FSocketSendOutput OnSuccess;
	// Error/Fail Blueprint Callback
	UPROPERTY(BlueprintAssignable)
		FSocketSendOutput OnFail;

	// Internal Functions/Delegates
private:
	// Internal on async complete callback delegate
	FSocketSendHandler myDelegate;
	// Starts the transmition/send task
	void Start(USocket *Socket, FString Message);
	// Function for handling the callback/on complete of the send task
	UFUNCTION()
		void HandleCallback(bool Success);

};
