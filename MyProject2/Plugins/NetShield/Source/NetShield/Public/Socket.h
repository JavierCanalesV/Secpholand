// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

// NOTE: The #define/#under related to UI is to avoid compilation issues where symbols existed more than once.
#define UI UI_ST
#include "openssl/ssl.h"
#undef UI

#include "Socket.generated.h"

/**
 * 
 */

// Class To Store Parameters Associated With A Socket Connection.
UCLASS(BlueprintType)
class NETSHIELD_API USocket : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	// Insecure TCP socket init
	void SetupSocket(int fileDescriptor);
	// Secure TCP socket init
	void SetupSocket(int fileDescriptor, SSL *ssl);

	// Returns the file descriptor of the socket
	int GetFileDescriptor();
	// Returns wether or not the socket is connected
	bool GetIsConnected();
	// Returns wether or not the socket is a secure (SSL/TLS) socket
	bool GetIsSSL();
	// Gets the SSL veriable
	SSL* GetSSLConnection();
	// Set the sockets' connection state to connected/disconnected
	void UpdateConnectionStatus(bool connected);

private:
	int fileDescriptor;
	SSL *ssl;
	bool isSSL;
	bool isConnected;
	
	
};
