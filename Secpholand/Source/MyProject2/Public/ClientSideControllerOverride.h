// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClientSideControllerOverride.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API AClientSideControllerOverride : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = ClientInfo)
		FString GetPlayerPort() const;

public:
	UFUNCTION(BlueprintCallable, Category = ClientInfo)
		void ClientTravelOverrided(const FString & URL, enum ETravelType TravelType, bool bSeamless, FGuid MapPackageGuid);
	
};
