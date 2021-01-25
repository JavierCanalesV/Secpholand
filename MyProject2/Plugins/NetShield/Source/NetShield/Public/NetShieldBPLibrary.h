// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "Engine.h"

#include "AESType.h"

#include "NetShieldBPLibrary.generated.h"

/* 
*	NetShield Blueprint Function Library
*
*	Takes care of tasks that can be done synchronously/on the main thread. Mainly includes myscelelanious functionality of NetShield like hashing, encoding, and encrypting.
*/
UCLASS()
class UNetShieldBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	// SHA Hashing
	// SHA-1 Hash
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SHA-1 hash", Keywords = "NetShield SHA SHA-1"), Category = "NetShield|SHA")
		static FString Sha1Hash(FString Message);
	// SHA-256 Hash - NOTE: This function is resource intensive, and may cause your thread to hang for a breef bit depending on your hardware. Use in a separate thread and/or with caution.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SHA-256 Hash", Keywords = "NetShield SHA SHA-256"), Category = "NetShield|SHA")
		static FString Sha256Hash(FString Message);
	// SHA-512 Hash - NOTE: This function is resource intensive, and may cause your thread to hang for a breef bit depending on your hardware. Use in a separate thread and/or with caution.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SHA-512 Hash", Keywords = "NetShield SHA SHA-256"), Category = "NetShield|SHA")
		static FString Sha512Hash(FString Message);

	// AES Cryptography
	// AES Encrypt - NOTE: This function is resource intensive, and may cause your thread to hang for a breef bit depending on your hardware. Use in a separate thread and/or with caution.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Aes CBC Encrypt", Keywords = "NetShield AES CBC Encrypt"), Category = "NetShield|AES")
		static TArray<uint8> AesEncrypt(EAESType Strength, FString Key, FString Iv, FString Message);
	// AES Decrypt - NOTE: This function is resource intensive, and may cause your thread to hang for a breef bit depending on your hardware. Use in a separate thread and/or with caution.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Aes CBC Decrypt", Keywords = "NetShield AES CBC Decrypt"), Category = "NetShield|AES")
		static FString AesDecrypt(EAESType Strength, FString Key, FString Iv, TArray<uint8> CipherText);

	// BASE64
	// Base64 Encode
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Base64 Encode", Keywords = "NetShield BASE64 Encode"), Category = "NetShield|Base64")
		static FString Base64Encode(TArray<uint8> byteArray);
	// Base64 Decode
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Base64 Decode", Keywords = "NetShield BASE64 Decode"), Category = "NetShield|Base64")
		static TArray<uint8> Base64Decode(FString encodedString);

	// Translation
	// FString -> Byte Array Translation
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Bytes", Keywords = "NetShield Bytes String"), Category = "NetShield|Base64")
		static TArray<uint8> StringToBytes(FString String);
	// Byte Array -> FString Translation
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes To String", Keywords = "NetShield Bytes String"), Category = "NetShield|Base64")
		static FString BytesToString(TArray<uint8> bytes);

	// Managed Socket Requests.
	// Managed socket close request
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Request Close", Keywords = "NetShield close managed managedclose"), Category = "NetShield|Managed")
		static void RequestManagedClose(class UManagedSocket *Socket);

	// Managed socket string send request
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Request Send Message", Keywords = "NetShield send managed message managedsend messagesend"), Category = "NetShield|Managed")
		static void RequestMMessageSend(class UManagedSocket *Socket, FString Message);

	// Managed socket string send request
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Request Send Bytes", Keywords = "NetShield send managed byte managedsend bytesend bytea byteasend"), Category = "NetShield|Managed")
		static void RequestMByteSend(class UManagedSocket *Socket, const TArray<uint8>& Bytes);

	// Gets whether or not a socket is (successfully) connected
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Socket Open?", Keywords = "NetShield socket open valid socketvalid isopen"), Category = "NetShield|Unmanaged")
		static bool GetSocketStatus(class USocket *Socket);

	// Internal Functions
	// AES Decrypt
	static int decrypt(EAESType Strength, unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
};
