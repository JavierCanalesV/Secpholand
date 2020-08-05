// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetworkGameModeOverrided.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API ANetworkGameModeOverrided : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable, Category = ClientInfo)
	FString GetPlayerIP() const;

	UFUNCTION(BlueprintCallable, Category = ClientInfo)
	FString GetPlayerUniqueID() const;

private:

	FString playerAddress;
	FString playerUniqueID;
	
};
