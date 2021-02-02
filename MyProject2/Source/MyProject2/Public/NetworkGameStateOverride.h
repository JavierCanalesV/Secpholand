// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetworkGameStateOverride.generated.h"





USTRUCT(BlueprintType)
struct FUserInfocpp {
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, Category = "FUser")
		int ID;
	UPROPERTY(BlueprintReadWrite, Category = "FUser")
		FString UserName;
	UPROPERTY(BlueprintReadWrite, Category = "FUser")
		FString UniqueID;
	UPROPERTY(BlueprintReadWrite, Category = "FUser")
		APlayerController* PlayerController;
};

UENUM(BlueprintType)
enum class EUserSearchType : uint8
{
	ID					UMETA(DisplayName = "ID"),
	UserName			UMETA(DisplayName = "UserName"),
	PlayerController    UMETA(DisplayName = "PlayerController"),
	UniqueID			UMETA(DisplayName = "UniqueID")

};


/**
 *
 */
UCLASS()
class MYPROJECT2_API ANetworkGameStateOverride : public AGameStateBase
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "GetUser")
		void GetUserCpp(UPARAM(ref) TArray<FUserInfocpp> & userStruct, const int ID, const FString Username, const FString UniqueID, const APlayerController* PlayerController, const TEnumAsByte<EUserSearchType> LookBye, bool & _found, struct FUserInfocpp & foundUser, int & Index);


};
