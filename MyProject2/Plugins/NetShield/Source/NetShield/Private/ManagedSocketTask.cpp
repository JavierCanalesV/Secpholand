// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "ManagedSocketTask.h"
#include "Async/Async.h"
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

// For polling is any data is available
#include <sys/ioctl.h>
#include <linux/sockios.h>
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
#include "openssl/pem.h"
#include "openssl/x509.h"
#include "openssl/x509_vfy.h"
#undef UI


UManagedSocketConnect* UManagedSocketConnect::CallbackLater(FString IP, int32 Port, bool Secure)
{
	UManagedSocketConnect* CallbackObject = NewObject<UManagedSocketConnect>();
	CallbackObject->Start(IP, Port, Secure);

	return CallbackObject;
}

void UManagedSocketConnect::Start(FString IP, int32 Port, bool Secure)
{
	// Bind the callback delegate to the HandleCallback function
	callbackDelegate.AddDynamic(this, &UManagedSocketConnect::HandleCallback);
	// Start an asynchronous connect task (that will call back on an error / when it's done)
	(new FAutoDeleteAsyncTask<FManagedSocketConnectionTask>(callbackDelegate, IP, Port, Secure))->StartBackgroundTask();
}

void UManagedSocketConnect::HandleCallback(bool Success, UManagedSocket* Socket)
{
	// Broadcast success/failure (and ensure the callbacks are run on the game thread)
	if (Success)
	{
		UE_LOG(LogNetShield, Verbose, TEXT("Successfully connected using Managed Socket!"))
		AsyncTask(ENamedThreads::GameThread, [=] {
			OnSuccess.Broadcast(Socket);
		});
	}
	else
	{
		UE_LOG(LogNetShield, Warning, TEXT("Unable to establish managed socket connection!"))
		AsyncTask(ENamedThreads::GameThread, [=] {
			OnFail.Broadcast(nullptr);
		});
	}
}

bool SendOverSocket(uint8_t *byteBuffer, int32 numBytes, UManagedSocket *Socket)
{
	// Get the sockets' file descriptor
	int fileDescriptor = Socket->GetFileDescriptor();

	// Verify the connection is active / the file descriptor is valid
	if (!Socket->GetIsConnected())
	{
		return false;
	}
	else if (fileDescriptor == -1)
	{
		return false;
	}

	// Is it an SSL socket?
	if (!Socket->GetIsSSL())
	{
		// Get the sockets' file descriptor
		int sock;
		if ((sock = Socket->GetFileDescriptor()) == -1)
		{
			return false;
		}
#ifdef _WIN32
		// Send() the data over the (insecure) TCP socket
		if (send(sock, (const char*)byteBuffer, numBytes, 0) == SOCKET_ERROR)
		{
			return false;
		}
#else
		// Send() the data over the (insecure) TCP socket
		if (send(sock, (const char *)byteBuffer, numBytes, 0) == -1)
		{
			return false;
		}
#endif

		// Broadcast success
		return true;
	}
	else
	{
		// Attempt to get the SSL object
		SSL *ssl = Socket->GetSSLConnection();
		if (ssl == nullptr)
		{
			return false;
		}

		// Attempt to write, and return false if it failed.
		if (SSL_write(ssl, byteBuffer, numBytes) <= 0)
		{
			return false;
		}

		// Otherwise assume the message successfully sent, and return success
		return true;
	}
}

TArray<uint8_t> TryReadSocket(UManagedSocket *Socket)
{
	// The socket receive size for a packet (will allow for 2048 bytes)
	int size = 2048;

	// Attempt to get the sockets' file descriptor and check that it is (still) connected
	int fileDescriptor = Socket->GetFileDescriptor();
	if (!Socket->GetIsConnected())
	{
		return TArray<uint8_t>();
	}
	if (fileDescriptor == -1)
	{
		return TArray<uint8_t>();
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
			// Free the char* buffer we allocated so we don't leak memory
			free(buff);

			return TArray<uint8_t>();
		}

		// Variable to store the ammount of buffer read
		int buffRead;
#ifdef _WIN32
		// Read the avaiblable buffer (and wait if it is not available)
		if ((buffRead = recv(sock, buff, size, 0)) == SOCKET_ERROR)
		{
			// Free the char* buffer we allocated so we don't leak memory
			free(buff);

			return TArray<uint8_t>();
		}
#else
		// Read the avaiblable buffer (and wait if it is not available)
		if ((buffRead = recv(sock, buff, size, 0)) == -1)
		{
			// Free the char* buffer we allocated so we don't leak memory
			free(buff);

			return TArray<uint8_t>();
		}
#endif

		// Buffer the *regular* buffer into a TArray buffer and return it.
		TArray<uint8_t> Buffer;

		for (int i = 0; i < buffRead; i++)
		{
			Buffer.Add(buff[i]);
		}

		// Free the char* buffer we allocated so we don't leak memory
		free(buff);

		return Buffer;
	}
	else
	{
		// Attempt to get the SSL object
		SSL *ssl = Socket->GetSSLConnection();

		// Check that the SSL object is a valid pointer
		if (ssl == nullptr)
		{
			// Free the char* buffer we allocated so we don't leak memory
			free(buff);

			return TArray<uint8_t>();
		}

		// Variable to store the ammount of buffer read
		int buffRead = SSL_read(ssl, buff, size);

		// Attempt to read the avaiblable buffer (and wait if it is not available)
		if (buffRead <= 0)
		{
			// Free the char* buffer we allocated so we don't leak memory
			free(buff);

			return TArray<uint8_t>();
		}

		// Buffer the *regular* buffer into a TArray buffer and return it.
		TArray<uint8_t> Buffer;

		for (int i = 0; i < buffRead; i++)
		{
			Buffer.Add(buff[i]);
		}

		// Free the char* buffer we allocated so we don't leak memory
		free(buff);

		return Buffer;

		// Null-terminate the buffer (won't segfault as we malloced max buffRead can be + 1)
		//buff[buffRead] = '\0';

		// Convert the buffer into an FString
		FString bufferString = ANSI_TO_TCHAR(buff);

		// Call the callback delegate with the buffer & success
		//return bufferString;
	}
}

void CloseSocket(UManagedSocket *Socket)
{
	if (!IsValid(Socket))
	{
		return;
	}

	int fileDescriptor = Socket->GetFileDescriptor();

	// Did some error occur/was the socket already closed?
	if (fileDescriptor == -1)
	{
		return;
	}

#ifdef _WIN32
	// Windows' closesocket and WSA cleanup
	closesocket(fileDescriptor);
	WSACleanup();
#else
	// Linux - just close()
	close(fileDescriptor);
#endif

	// Successfully closed socket!
	UE_LOG(LogNetShield, Verbose, TEXT("Successfully closed socket connection!"));
}

// Attempt to connect
void FManagedSocketConnectionTask::DoWork()
{
	// Connection functionality
	{
		// If the platform type is win32, start the winsock library
#ifdef _WIN32
		WSADATA wsaData;

		int iResult;
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			UE_LOG(LogNetShield, Warning, TEXT("WSAStartup failed with error code: %i"), iResult);
			MyInput.Broadcast(false, nullptr);
			return;
		}
#endif

		// Create a TCP socket
		int sock = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);

		// Prepare a connection to the IP and Port
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		// To avoid the winsock2 depricated inet_addr error
#ifdef _WIN32
		inet_pton(AF_INET, TCHAR_TO_ANSI(*IP), &(clientService.sin_addr.s_addr));
#else
		clientService.sin_addr.s_addr = inet_addr(TCHAR_TO_ANSI(*IP));
#endif
		clientService.sin_port = htons(Port);

		// If it's WIN32 (winsock) use SOCKADDR, otherwise use the BSD strufct sockaddr
#ifdef _WIN32
		// Attempt to connect to the IP/Port
		if (connect(sock, (SOCKADDR *)&clientService, sizeof(clientService)) != 0)
		{
			UE_LOG(LogNetShield, Error, TEXT("Failed to create connection!"));
			MyInput.Broadcast(false, nullptr);
			return;
		}
#else
		// Attempt to connect to the IP/Port
		if (connect(sock, (struct sockaddr *)&clientService, sizeof(clientService)) != 0)
		{
			UE_LOG(LogNetShield, Error, TEXT("Failed to create connection!"));
			MyInput.Broadcast(false, nullptr);
			return;
		}
#endif

		// If it's a regular (unsecured) TCP connection, create a socket object and return it
		if (Secure)
		{
			// Initialize openssl
			OpenSSL_add_all_algorithms();
			ERR_load_BIO_strings();
			ERR_load_crypto_strings();
			SSL_load_error_strings();
			BIO *certbio = BIO_new(BIO_s_file());
			BIO *outbio = BIO_new_fp(stdout, BIO_NOCLOSE);

			// Attempt to init the SSL library
			if (SSL_library_init() < 0)
				BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

			// Support everythng except SSLv1 (we'll disable SSLv2 later)
			const SSL_METHOD *method = SSLv23_client_method();

			// Attempt to create the SSL context structure
			SSL_CTX *ctx;
			if ((ctx = SSL_CTX_new(method)) == NULL)
			{
				UE_LOG(LogNetShield, Warning, TEXT("Error creating SSL context structure!"))
					MyInput.Broadcast(false, nullptr);
				return;
			}

			// Disable SSLv2 (as that is considered insecure
			SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

			// Create an SSL object based upon the context and set it's file descriptor to the sockets' file descriptor
			SSL *ssl = SSL_new(ctx);
			SSL_set_fd(ssl, sock);

			// Attempt to SSL connect / complete the SSL handshake
			if (SSL_connect(ssl) != 1)
			{
				UE_LOG(LogNetShield, Warning, TEXT("Failed to create SSL session. Does your server support SSLv3/TLS?"));
				return MyInput.Broadcast(false, nullptr);
			}

			// Retrieve the servers' certificate
			X509 *cert = SSL_get_peer_certificate(ssl);

			// Verify we got a certificate back
			if (cert == NULL)
			{
				UE_LOG(LogNetShield, Warning, TEXT("Failed to retrieve certificate from server."));
				MyInput.Broadcast(false, nullptr);
				return;
			}

			// Extract the name from the certificate
			X509_NAME *certname = X509_NAME_new();
			certname = X509_get_subject_name(cert);

			// TODO - Future functionality: Expose certificate information to the blueprint library

			// Free the now unrequired variables
			X509_free(cert);
			SSL_CTX_free(ctx);

			// Create & setup a socket with the SSL object and file descriptor
			mySocket = NewObject<UManagedSocket>();
			mySocket->SetupSocket(sock, ssl);

			// Set the socket's manager to this task/instance.
			mySocket->Manager = this;

			// Set the socket's READ timeout to be 100ms, which means that read calls aren't blocking and we can properly try to get any buffers, without it messing up any of the requests like closing and/or sending.
			struct timeval interval;
			interval.tv_sec = 1;
			interval.tv_usec = 100;
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

			// Return success/the SSL connections' socket
			MyInput.Broadcast(true, mySocket);
		}
		else
		{
			// "Regular"/unsecured TCP socket, so no need to do any of the SSL complications.
			mySocket = NewObject<UManagedSocket>();
			mySocket->SetupSocket(sock);

			// Set the socket's manager to this task/instance.
			mySocket->Manager = this;

			// Set the socket's READ timeout to be 100ms, which means that read calls aren't blocking and we can properly try to get any buffers, without it messing up any of the requests like closing and/or sending.
			struct timeval interval;
			interval.tv_sec = 1;
			interval.tv_usec = 100;
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

			MyInput.Broadcast(true, mySocket);
		}
	}

	// Loop functionality - will check for new byteBufferers (in and out), and will act accodingly.
	while (bIsSocketActive && IsValid(mySocket))
	{
		// Is sending of data requested?
		if (mySocket->bShouldFlush && mySocket->buffer.Num() > 0)
		{
			int bufferSize = mySocket->buffer.Num();
			// Allocate a byte buffer we can copy the TArray in to for sending it off.
			uint8_t *buffer;

			// Set the buffer to the head of the TArray.
			buffer = mySocket->buffer.GetData();

			SendOverSocket(buffer, bufferSize, mySocket);

			// Reset the buffer/should flush status as we just flushed the buffer. Assume that no further data has been added while sending, but even it it was, it should have been sent since we're setting the pointer to the head of the TArray.
			mySocket->buffer.Empty();
			mySocket->bShouldFlush = false;

			// TODO Should we have a callback / an interface a LOT like SocketSend?
		}

		TArray<uint8_t> ReadData;
		// Is there any inbound data pending?
		if ((ReadData = TryReadSocket(mySocket)).Num() > 0)
		{
			// Check if the bytes handler is bound, and (try to) call it if it is.
			if (mySocket->OnReceiveBytes.IsBound())
			{
				// Execute the event on the game thread so a user can do (pretty much) whatever they desire without hitting issues. Issues mainly occur when doing things like spawning widgets/actors.
				AsyncTask(ENamedThreads::GameThread, [=] {
					mySocket->OnReceiveBytes.Broadcast(ReadData);
				});
			}

			// Check if the message handler is bound, and (try to) call it if it is (parsing the byte array into a string). NOTE: Both Bytes and Message can be bound, and should thus both be called.
			if (mySocket->OnReceiveMessage.IsBound())
			{
				// Add a null terminating byte to the array so we won't get any memory/seg errors/etc.
				ReadData.Add((uint8_t)'\0');

				// Convert the data into an FString assuming UTF8-encoding (NOTE: UTF8 is "backwards" compatible with ASCII; data may not be valid depending on server implementation)
				FString ReadDataString = UTF8_TO_TCHAR((const char *)ReadData.GetData());

				// Execute the event on the game thread so a user can do (pretty much) whatever they desire without hitting issues. Issues mainly occur when doing things like spawning widgets/actors.
				AsyncTask(ENamedThreads::GameThread, [=] {
					mySocket->OnReceiveMessage.Broadcast(ReadDataString);
				});
			}
		}
	}

	CloseSocket(mySocket);

	// Now ensure the socket's connected status is set to not connected since we're done.
	if (mySocket)
	{
		mySocket->UpdateConnectionStatus(false);
	}
}

void UManagedSocket::BeginDestroy()
{
	if (Manager)
	{
		// Request a stop of the socket connection by setting it's is active variable to false
		Manager->bIsSocketActive = false;
		
		UE_LOG(LogNetShield, Log, TEXT("Closing socket connection due to socket object being destoroyed..."));
	}
	else
	{
		UE_LOG(LogNetShield, Warning, TEXT("Managed Socket's Manager task appears to be invalid!"));
	}

	// Call the parent's BeginDestroy so the default/expected object destruction behavior happens (too).
	Super::BeginDestroy();
}

#if WITH_EDITOR
void UManagedSocket::HandleExitPIE(bool someBool)
{
	//BeginDestroy();
	if (Manager)
	{
		// Request a stop of the socket connection by setting it's is active variable to false
		Manager->bIsSocketActive = false;

		UE_LOG(LogNetShield, Log, TEXT("Closing socket connection due to PIE ending..."));
	}
	else
	{
		UE_LOG(LogNetShield, Warning, TEXT("Managed Socket's Manager task appears to be invalid!"));
	}

	// Reset the delegate (avoids potential crashes after launching PIE many times
	FEditorDelegates::EndPIE.Remove(EndPIEDelegate);
}
#endif
