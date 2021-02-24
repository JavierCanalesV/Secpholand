// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#include "NetShieldBPLibrary.h"
#include "NetShield.h"

// OpenSSL Includes
// NOTE: The #define/#under related to UI is to avoid compilation issues where symbols existed more than once.
#define UI UI_ST
#include "openssl/sha.h"
#include "openssl/aes.h"
#include "openssl/evp.h"
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

// Include the managed socket task so we have access to the managed socket class/object
#include "ManagedSocketTask.h"

UNetShieldBPLibrary::UNetShieldBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

// Hashes a string using SHA-1 (very inscure!)
FString UNetShieldBPLibrary::Sha1Hash(FString Message)
{
	// Create a byte array (and set it to \0) to store the digest
	unsigned char digest[SHA_DIGEST_LENGTH];
	memset(digest, 0x0, SHA_DIGEST_LENGTH);

	// Create the SHA-1 digest (and store it in digest)
	SHA1((unsigned char *)FTCHARToUTF8((const TCHAR*)*Message).Get(), strlen(FTCHARToUTF8((const TCHAR*)*Message).Get()), digest);

	// Build up the digest string
	FString digestString;
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
		digestString.Append(FString::Printf(TEXT("%02x"), digest[i]));

	return digestString;
}

// Hashes a string using SHA-256
FString UNetShieldBPLibrary::Sha256Hash(FString Message)
{
	// Create a byte array (and set it to \0) to store the digest
	unsigned char digest[SHA256_DIGEST_LENGTH];
	memset(digest, 0x0, SHA256_DIGEST_LENGTH);

	// Convert the FString into a char* (UTF8-compatible)
	int size = strlen(FTCHARToUTF8((const TCHAR*)*Message).Get());

	// Create the SHA-1 digest (and store it in digest)
	SHA256((unsigned char *)FTCHARToUTF8((const TCHAR*)*Message).Get(), size, digest);
	
	// Build up the digest string
	FString digestString;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
		digestString.Append(FString::Printf(TEXT("%02x"), digest[i]));
	
	return digestString;
}

// Hashes a string using SHA-512
FString UNetShieldBPLibrary::Sha512Hash(FString Message)
{
	// Create a byte array (and set it to \0) to store the digest
	unsigned char digest[SHA512_DIGEST_LENGTH];
	memset(digest, 0x0, SHA512_DIGEST_LENGTH);

	// Create the SHA-1 digest (and store it in digest)
	SHA512((unsigned char *)FTCHARToUTF8((const TCHAR*)*Message).Get(), strlen(FTCHARToUTF8((const TCHAR*)*Message).Get()), digest);

	// Build up the digest string
	FString digestString;
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		digestString.Append(FString::Printf(TEXT("%02x"), digest[i]));

	return digestString;
}

// Encrypts an FString using AES
TArray<uint8> UNetShieldBPLibrary::AesEncrypt(EAESType Strength, FString Key, FString Iv, FString Message)
{
	// Initialize a "null" IV by default, and only override it if an IV is provided (NOTE: using the Null in a string can cause issues)
	unsigned char IvBytes[32] = {0};
	if (!Iv.IsEmpty())
	{
		// Copy over the passed in IV to the byte array
		memcpy(IvBytes, (unsigned char*)FTCHARToUTF8((const TCHAR*)*Iv).Get(), FMath::Clamp(0, Iv.Len(), 32));
	}
	// Calculate the length of the AES string (blocks of 16 (AES's blocksize))
	int encryptedLength = (strlen((char *)FTCHARToUTF8((const TCHAR*)*Message).Get()) / 16 + 1) * 16;

	// Create a buffer to store the ciphertext
	unsigned char *cipherText = (unsigned char *)malloc(encryptedLength);
	int cipherTextLen;

	// Setup the crypto library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);

	// Create the cipher context
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

	// Intermediate variable to store the length of the ciphertext
	int len;

	// Check if the cipher context was invalid
	if (!ctx)
		return TArray<uint8>();

	// Attempt to initialize the cipher suite with AES 128 or AES 256
	if (Strength == EAESType::AT_128)
	{
		// Check if they keys length is 128 bits
		if (strlen((char *)FTCHARToUTF8((const TCHAR*)*Key).Get()) * 8 != 128)
		{
			UE_LOG(LogNetShield, Error, TEXT("Your key needs to be 128 bits (16 characters) long for AES-128!"))
			return TArray<uint8>();
		}

		// Attempt to initialize the EVP crypto function with 128 bit CBC AES
		if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (const unsigned char*)FTCHARToUTF8((const TCHAR*)*Key).Get(), IvBytes))
			return TArray<uint8>();
	}
	else if (Strength == EAESType::AT_256)
	{
		// Check if they keys length is 256 bits
		if (strlen((char *)FTCHARToUTF8((const TCHAR*)*Key).Get()) * 8 != 256)
		{
			UE_LOG(LogNetShield, Error, TEXT("Your key needs to be 256 bits (32 characters) long for AES-256!"))
			return TArray<uint8>();
		}

		// Attempt to initialize the EVP crypto function with 256 bit CBC AES
		if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)FTCHARToUTF8((const TCHAR*)*Key).Get(), IvBytes))
			return TArray<uint8>();
	}
	else
	{
		// Somehow an extra enum got added that we wheren't able to handle!
		UE_LOG(LogNetShield, Error, TEXT("AES Encrypt doesn't support this key length!"))
		return TArray<uint8>();
	}

	// Encrypt the plaintext into cipherText
	if (1 != EVP_EncryptUpdate(ctx, cipherText, &len, (const unsigned char*)FTCHARToUTF8((const TCHAR*)*Message).Get(), strlen((char *)FTCHARToUTF8((const TCHAR*)*Message).Get())))
		return TArray<uint8>();

	cipherTextLen = len;

	// Finalize the encryption
	if (1 != EVP_EncryptFinal_ex(ctx, cipherText + len, &len))
		return TArray<uint8>();

	cipherTextLen += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	// Convert the ciphertext into an array of BYTEs
	TArray<uint8> byteArray;
	for (int i = 0; i < encryptedLength; i++)
		byteArray.Add((uint8)cipherText[i]);

	// Cleanup/free the crypto library
	EVP_cleanup();
	ERR_free_strings();

	return byteArray;
}

FString UNetShieldBPLibrary::AesDecrypt(EAESType Strength, FString Key, FString Iv, TArray<uint8> CipherText)
{
	// Initialize a "null" IV by default, and only override it if an IV is provided (NOTE: using the Null in a string can cause issues)
	unsigned char IvBytes[32] = { 0 };
	if (!Iv.IsEmpty())
	{
		// Copy over the passed in IV to the byte array
		memcpy(IvBytes, (unsigned char*)FTCHARToUTF8((const TCHAR*)*Iv).Get(), FMath::Clamp(0, Iv.Len(), 32));
	}

	// Copy the ciphertext to the byte array (unsigned char *)
	unsigned char *cipherText = (unsigned char *)malloc(CipherText.Num());
	for (int i = 0; i < CipherText.Num(); i++)
		cipherText[i] = (unsigned char)CipherText[i];

	// Create a byte buffer for the decrypted text (and account for the worst case, no padding scenario to avoid a buffer overflow)
	unsigned char *plainText = (unsigned char *)malloc(CipherText.Num());
	int plainTextLen, cipherTextLen = CipherText.Num();

	// Initialise the library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);

	// Decrypt the ciphertext
	plainTextLen = decrypt(Strength, cipherText, cipherTextLen, (unsigned char *)FTCHARToUTF8((const TCHAR*)*Key).Get(), IvBytes, plainText);

	// Null-terminate the decrypted buffer
	plainText[plainTextLen] = '\0';

	// Return the FString representation of the plain text
	return FString(UTF8_TO_TCHAR((char *)plainText));
}

// Turns a byte array into a base64 encoded FString
FString UNetShieldBPLibrary::Base64Encode(TArray<uint8> byteArray)
{
	// To avoid encoding nothing (and causing a bug)
	if (byteArray.Num() == 0)
		return FString();

	// Convert the TArray<uint8> into a usable unsigned char array
	unsigned char *bytez = (unsigned char *)malloc(byteArray.Num());

	for (int i = 0; i < byteArray.Num(); i++)
		bytez[i] = byteArray[i];

	// Create base64 specific variables
	BIO *bio, *base64Bio;
	BUF_MEM *bptr;

	// Base64 encode the bytez array
	base64Bio = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	base64Bio = BIO_push(base64Bio, bio);
	BIO_write(base64Bio, bytez, byteArray.Num());
	BIO_flush(base64Bio);
	BIO_get_mem_ptr(base64Bio, &bptr);

	// Copy it over into a string buffer (ande null-terminate the string)
	char *buff = (char *)malloc(bptr->length);
	memcpy(buff, bptr->data, (bptr->length) - 1);
	buff[bptr->length - 1] = 0;

	// Free the base64 object
	BIO_free_all(base64Bio);

	// Convert the base64 string buffer (buff) into an FString
	FString cipherText = FString(ANSI_TO_TCHAR(buff));
	return cipherText;
}

TArray<uint8> PerformBASE64Decode(const char *base64String)
{
	// NOTE: We're doing this in a seperate function to avoid the temporary pointer limit. The below code will decode the BASE64 string, and return the resulting bytes in a TArray.
	const char *str = base64String;
	BIO *b64(BIO_new(BIO_f_base64()));
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO* source = BIO_new_mem_buf(str, -1); // read-only source
	BIO_push(b64, source);
	const int maxlen = strlen(str) / 4 * 3 + 1;
	uint8_t *buffer = (uint8_t*)malloc(maxlen);
	const int len = BIO_read(b64, buffer, maxlen);

	TArray<uint8> byteArray;
	for (int i = 0; i < len; i++)
		byteArray.Add(buffer[i]);

	BIO_free_all(b64);
	free(buffer);

	return byteArray;
}

// Turns a base64 encoded FString into a byte array
TArray<uint8> UNetShieldBPLibrary::Base64Decode(FString encodedString)
{
	//return PerformBASE64Decode("eyJUZXN0SW50IjogNDQsDQogICAgIlRlc3RCb29sIjogdHJ1ZSwNCiAgICAiVGVzdEZsb2F0IjogNDUwLjU1NDk5MjY3NTc4MTI1LA0KICAgICJUZXN0U3RyaW5nIjogIkhlbGxvIGhlbGxvIiwNCiAgICAiUGFja2V0SWQiOiAwDQp9\n");

	FString EncodedNewlineString = encodedString + TEXT("\n");
	encodedString.ReplaceInline(TEXT("\n"), TEXT(""));
	encodedString = encodedString + TEXT("\n");
	auto StringCasted = StringCast<ANSICHAR>(*encodedString);
	const char *str = StringCasted.Get();
	//char *str2 = (char*)malloc(encodedString.Len() + 1);
	//memcpy(str2, str, encodedString.Len() + 1);
	//return PerformBASE64Decode(StringCasted.Get());

	BIO *b64(BIO_new(BIO_f_base64()));
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO* source = BIO_new_mem_buf(str, -1); // read-only source
	BIO_push(b64, source);
	const int maxlen = strlen(str) / 4 * 3 + 1;
	uint8_t *buffer = (uint8_t*)malloc(maxlen);
	const int len = BIO_read(b64, buffer, maxlen);

	TArray<uint8> byteArray;
	for (int i = 0; i < len; i++)
		byteArray.Add(buffer[i]);

	BIO_free_all(b64);
	free(buffer);

	return byteArray;
	//return PerformBASE64Decode(StringCasted.Get());
}

// Converts a string to a byte array
TArray<uint8> UNetShieldBPLibrary::StringToBytes(FString String)
{
	// Turn the FString input into a char* string
	auto StringCasted = StringCast<ANSICHAR>(*String);
	const char *input = StringCasted.Get();

	// Itterate over each character in the string and add it as a byte
	TArray<uint8> bytes;
	for (int i = 0; i < strlen(input); i++)
		bytes.Add((unsigned char)input[i]);

	return bytes;
}

// Converts a byte array to a string
FString UNetShieldBPLibrary::BytesToString(TArray<uint8> bytes)
{
	// Create an empty string to hold the result
	FString store = "";

	// Itterate over the byte array, appending the characters to the store string
	for (int i = 0; i < bytes.Num(); i++)
		store.AppendChar((unsigned char)bytes[i]);

	return store;
}

void UNetShieldBPLibrary::RequestManagedClose(UManagedSocket *Socket)
{
	if (IsValid(Socket) && Socket->Manager)
	{
		// "Request" the stop by setting it's active to false. This will cause the task to halt/exit on the next iteration (which should be in less than a second of request).
		Socket->Manager->bIsSocketActive = false;
	}
}

void UNetShieldBPLibrary::RequestMMessageSend(UManagedSocket *Socket, FString Message)
{
	// Convert the string into a byte array, and then call the byte send function to avoid duplicating the buffer adding code.
	const char *CMessage = FTCHARToUTF8((const TCHAR*)*Message).Get();

	if (CMessage == NULL)
	{
		// Error! Pointer doesn't appear to be valid, thus there must have been some form of a conversion mistake.
		return;
	}
	// Length of the string/char* buffer
	int l = FMath::Min((int32)strlen(CMessage), Message.Len());

	TArray<uint8_t> ByteBuffer;

	for (int i = 0; i < l; i++)
	{
		ByteBuffer.Add(CMessage[i]);
	}

	// Pass it along to the byte buffer sender
	RequestMByteSend(Socket, ByteBuffer);
}

void UNetShieldBPLibrary::RequestMByteSend(UManagedSocket *Socket, const TArray<uint8_t>& Bytes)
{
	if (IsValid(Socket))
	{
		/// Assume it's an outgoing queue, and the user won't call them twice. In the really unlikely case a "race condition" does occur, we will mitigate it by simply adding the requested buffer to the array, thus avoiding the lock of the main (game) thread 
		/// That you'd get when using an approach involving mutexes.

		// Add the to be sent buffer to the outgoing buffer, and request a flush on the next socket "tick"/iteration.
		Socket->buffer.Append(Bytes);
		Socket->bShouldFlush = true;
	}
}

bool UNetShieldBPLibrary::GetSocketStatus(class USocket *Socket)
{
	// Ensure the socket is valid
	if (!Socket)
	{
		return false;
	}

	// Use the socket object's own method to determine whether or not the socket is still open/connected.
	return Socket->GetIsConnected();
}

// Internal function for decrypting AES
int UNetShieldBPLibrary::decrypt(EAESType Strength, unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
	int len;
	int plainTextLen;

	// Create a cipher context
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		return -1;

	// Initialize a the correct bit AES CBC context
	// NOTE: We can use a ternary operator since there's only two options (either 128 bit or 256 bit; should this change in the future, we might instead want to extract this out
	// and use a switch (or an if-else if-else fork to initialize based on strength)
	if (1 != EVP_DecryptInit_ex(ctx, Strength == EAESType::AT_128 ? EVP_aes_128_cbc() : EVP_aes_256_cbc(), NULL, key, iv))
		return -1;

	// Add the message to the decryption context
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		return -1;

	plainTextLen = len;

	// Finalize the decryption
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		return -1;

	// Add any extra length that could have occured
	plainTextLen += len;

	// Free the context
	EVP_CIPHER_CTX_free(ctx);

	return plainTextLen;
}