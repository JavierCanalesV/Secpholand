// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "SocketReceive.h"
#include "NetShield.h"

// Socket Library Headers
#ifdef _WIN32
// Winsock Includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "Windows/HideWindowsPlatformTypes.h"

#pragma comment(lib, "Ws2_32.lib")
#else
// BSD Socket Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

// OpenSSL Includes
// NOTE: The #define/#under related to UI is to avoid compilation issues where symbols existed more than once.
#define UI UI_ST
#include "openssl/conf.h"
#include "openssl/err.h"
#include "openssl/bio.h"
#include "openssl/hmac.h"
#include "openssl/buffer.h"
#include "openssl/ssl.h"
#undef UI

USocketReceive* USocketReceive::CallbackLater(USocket *Socket)
{
	// Create an instance of this class and start the async receive task
	USocketReceive* CallbackObject = NewObject<USocketReceive>();
	CallbackObject->Start(Socket);

	return CallbackObject;
}

void USocketReceive::Start(USocket *Socket)
{
	// Bind myDelegate to the HandleCallBack function for when the receive completes
	myDelegate.AddDynamic(this, &USocketReceive::HandleCallback);

	// Start a FSocketReceiveTask with the previously bound delegate and passed in socket
	FAutoDeleteAsyncTask<FSocketReceiveTask> *myTask = (new FAutoDeleteAsyncTask<FSocketReceiveTask>(myDelegate, Socket));
	myTask->StartBackgroundTask();

	return;
}

void USocketReceive::HandleCallback(bool Success, FString Message)
{
	// Call the success or fail delegate depending on wether on not the receive was a success
	if (Success)
		OnSuccess.Broadcast(Message);
	else
		OnFail.Broadcast(Message);
}

// Attempt to connect
void FSocketReceiveTask::DoWork()
{
	// The socket receive size for a packet (will allow for 2048 bytes)
	int size = 2048;

	// Attempt to get the sockets' file descriptor and check that it is (still) connected
	int fileDescriptor = Socket->GetFileDescriptor();
	if (!Socket->GetIsConnected())
	{
		MyInput.Broadcast(false, FString());
		return;
	}
	if (fileDescriptor == -1)
	{
		MyInput.Broadcast(false, FString());
		return;
	}

	// Create buffer to hold incoming data
	char *buff = (char *)malloc(size + 1);

	// Is it an SSL socket?
	if (!Socket->GetIsSSL())
	{
		// Variable to store the sockets' file descriptor
		int sock;

		// Could it successfully get the sockets' file descriptor?
		if ((sock = Socket->GetFileDescriptor()) == -1)
		{
			MyInput.Broadcast(false, FString());
			return;
		}

		// Variable to store the ammount of buffer read
		int buffRead;
#ifdef _WIN32
		// Read the avaiblable buffer (and wait if it is not available)
		if ((buffRead = recv(sock, buff, size, 0)) == SOCKET_ERROR)
		{
			MyInput.Broadcast(false, FString());
			return;
		}
#else
		// Read the avaiblable buffer (and wait if it is not available)
		if ((buffRead = recv(sock, buff, size, 0)) == -1)
		{
			MyInput.Broadcast(false, FString());
			return;
		}
#endif

		// Null-terminate the buffer (won't segfault as we malloced max buffRead can be + 1)
		buff[buffRead] = '\0';

		// Convert the buffer into an FString
		FString bufferString = ANSI_TO_TCHAR(buff);

		// Return success with the buffer
		MyInput.Broadcast(true, bufferString);
		return;
	}
	else
	{
		// Attempt to get the SSL object
		SSL *ssl = Socket->GetSSLConnection();

		// Check that the SSL object is a valid pointer
		if (ssl == nullptr)
		{
			MyInput.Broadcast(false, FString());
			return;
		}

		// Variable to store the ammount of buffer read
		int buffRead;

		// Attempt to read the avaiblable buffer (and wait if it is not available)
		if ((buffRead = SSL_read(ssl, buff, size)) <= 0)
		{
			MyInput.Broadcast(false, FString());
			return;
		}

		// Null-terminate the buffer (won't segfault as we malloced max buffRead can be + 1)
		buff[buffRead] = '\0';

		// Convert the buffer into an FString
		FString bufferString = ANSI_TO_TCHAR(buff);

		// Call the callback delegate with the buffer & success
		MyInput.Broadcast(true, bufferString);
		return;
	}
}