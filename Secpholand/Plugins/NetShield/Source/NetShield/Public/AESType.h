// Copyright 2017-2020 HowToCompute. All Rights Reserved.

#pragma once

#include "Engine/UserDefinedEnum.h"
#include "AESType.generated.h"

// Enum to store how many bits of AES encryption the plugin should use (AT_128 -> 128bit; AT_256->256bit)
UENUM(BlueprintType)
enum class EAESType : uint8
{
	AT_128 	UMETA(DisplayName = "128-bit AES"),
	AT_256 	UMETA(DisplayName = "256-bit AES")
};