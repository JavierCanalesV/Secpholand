// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "Socket.h"
#include "NetShield.h"


USocket::USocket(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	isConnected = false;
	isSSL = false;
	fileDescriptor = -1;
	ssl = nullptr;
}

void USocket::SetupSocket(int _fileDescriptor)
{
	isConnected = true;
	isSSL = false;
	fileDescriptor = _fileDescriptor;
}

void USocket::SetupSocket(int _fileDescriptor, SSL* _ssl)
{
	isConnected = true;
	isSSL = true;
	fileDescriptor = _fileDescriptor;
	ssl = _ssl;
}

int USocket::GetFileDescriptor()
{
	return isConnected ? fileDescriptor : -1;
}

bool USocket::GetIsConnected()
{
	return isConnected;
}

bool USocket::GetIsSSL()
{
	return isSSL;
}

SSL* USocket::GetSSLConnection()
{
	// Verify the socket is of the SSL type and is connected
	if (!isSSL || !isConnected)
		return nullptr;
	return ssl;
}

void USocket::UpdateConnectionStatus(bool connected)
{
	isConnected = false;
}