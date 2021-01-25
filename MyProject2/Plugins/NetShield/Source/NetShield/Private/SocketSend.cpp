// Copyright 2017-2020 HowToCompute. All Rights Reserved.


#include "SocketSend.h"
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

USocketSend* USocketSend::CallbackLater(USocket *Socket, FString Message)
{
	// Create an instance of this class (for the callback delegates)
	USocketSend* CallbackObject = NewObject<USocketSend>();
	// Call the start function on the previously instantiated class to get the sending to start.
	CallbackObject->Start(Socket, Message);

	return CallbackObject;
}

void USocketSend::Start(USocket *Socket, FString Message)
{
	// Bind the callback delegate to the HandlerCallback function
	myDelegate.AddDynamic(this, &USocketSend::HandleCallback);
	// Start the async send task
	(new FAutoDeleteAsyncTask<FSocketSendTask>(myDelegate, Socket, Message))->StartBackgroundTask();
	return;
}

void USocketSend::HandleCallback(bool Success)
{
	// Call the success/fail delegate based on wether or not it successfully sent
	if (Success)
		OnSuccess.Broadcast();
	else
		OnFail.Broadcast();
}

// Attempt To Send
void FSocketSendTask::DoWork()
{
	// Get the sockets' file descriptor
	int fileDescriptor = Socket->GetFileDescriptor();

	// Verify the connection is active / the file descriptor is valid
	if (!Socket->GetIsConnected())
	{
		MyInput.Broadcast(false);
		return;
	}
	else if (fileDescriptor == -1)
	{
		MyInput.Broadcast(false);
		return;
	}

	// Create a char* buffer from the Message FString
	char *buff = TCHAR_TO_ANSI(*Message);

	// Is it an SSL socket?
	if (!Socket->GetIsSSL())
	{
		// Get the sockets' file descriptor
		int sock;
		if ((sock = Socket->GetFileDescriptor()) == -1)
		{
			MyInput.Broadcast(false);
			return;
		}
#ifdef _WIN32
		// Send() the data over the (insecure) TCP socket
		if (send(sock, buff, strlen(buff), 0) == SOCKET_ERROR)
		{
			MyInput.Broadcast(false);
			return;
		}
#else
		// Send() the data over the (insecure) TCP socket
		if (send(sock, buff, strlen(buff), 0) == -1)
		{
			MyInput.Broadcast(false);
			return;
		}
#endif

		// Broadcast success
		MyInput.Broadcast(true);
		return;
	}
	else
	{
		// Attempt to get the SSL object
		SSL *ssl = Socket->GetSSLConnection();
		if (ssl == nullptr)
		{
			MyInput.Broadcast(false);
			return;
		}

		// Attempt to write, and return false if it failed.
		if (SSL_write(ssl, buff, (int)strlen(buff)) <= 0)
		{
			MyInput.Broadcast(false);
			return;
		}

		// Otherwise assume the message successfully sent, and return success
		MyInput.Broadcast(true);
		return;
	}
}