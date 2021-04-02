// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "SocketClose.h"
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

USocketClose* USocketClose::CallbackLater(USocket *Socket)
{
	// Create a new instance of this class (for OnSuccess/OnFail callback
	USocketClose* CallbackObject = NewObject<USocketClose>();
	// Call the start function in the previously created class
	CallbackObject->Start(Socket);

	return CallbackObject;
}

void USocketClose::Start(USocket *Socket)
{
	// Bind the delegate to the HandleCallback function
	myDelegate.AddDynamic(this, &USocketClose::HandleCallback);
	// Start the asynchronous socket close task
	(new FAutoDeleteAsyncTask<FSocketCloseTask>(myDelegate, Socket))->StartBackgroundTask();
	return;
}

void USocketClose::HandleCallback(bool Success)
{
	// Return OnSuccess/OnFail based on wether or not we successfully closed the socket.
	if (Success)
		OnSuccess.Broadcast();
	else
		OnFail.Broadcast();
}

// Attempt to close the connection
void FSocketCloseTask::DoWork()
{
	int fileDescriptor = Socket->GetFileDescriptor();

	// Did some error occur/was the socket already closed?
	if (fileDescriptor == -1)
		MyInput.Broadcast(false);

#ifdef _WIN32
	// Windows' closesocket and WSA cleanup
	closesocket(fileDescriptor);
	WSACleanup();
#else
	// Linux - just close()
	close(fileDescriptor);
#endif

	// Set the sockets' status to disconnected and return success
	Socket->UpdateConnectionStatus(false);
	MyInput.Broadcast(true);
}
