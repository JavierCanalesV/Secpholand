// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "SocketConnect.h"
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
#include "openssl/pem.h"
#include "openssl/x509.h"
#include "openssl/x509_vfy.h"
#undef UI


USocketConnect* USocketConnect::CallbackLater(FString IP, int32 Port, bool Secure)
{
	USocketConnect* CallbackObject = NewObject<USocketConnect>();
	CallbackObject->Start(IP, Port, Secure);

	return CallbackObject;
}

void USocketConnect::Start(FString IP, int32 Port, bool Secure)
{
	// Bind the callback delegate to the HandleCallback function
	myDelegate.AddDynamic(this, &USocketConnect::HandleCallback);
	// Start an asynchronous connect task (that will call back on an error / when it's done)
	(new FAutoDeleteAsyncTask<FMyTaskName>(myDelegate, IP, Port, Secure))->StartBackgroundTask();
}

void USocketConnect::HandleCallback(bool Success, USocket* Socket)
{
	// Broadcast success/failure
	if (Success)
		OnSuccess.Broadcast(Socket);
	else
		OnFail.Broadcast(nullptr);
}

// Attempt to connect
void FMyTaskName::DoWork()
{

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

		// If it's a regular (unsecured) TCP connection, create a USocket object and return it
		if (!Secure)
		{
			USocket *mySocket = NewObject<USocket>();
			mySocket->SetupSocket(sock);
			MyInput.Broadcast(true, mySocket);
			return;
		}

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
		USocket *mySocket = NewObject<USocket>();
		mySocket->SetupSocket(sock, ssl);

		// Return success/the SSL connections' socket
		MyInput.Broadcast(true, mySocket);
	}
}
