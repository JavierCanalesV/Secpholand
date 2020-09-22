// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSideControllerOverride.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

FString AClientSideControllerOverride::GetPlayerPort() const
{
	return GetWorld()->GetAddressURL();
}

void AClientSideControllerOverride::ClientTravelOverrided(const FString & URL, enum ETravelType TravelType, bool bSeamless, FGuid MapPackageGuid)
{
	ClientTravel(URL, TravelType, bSeamless, MapPackageGuid);
}

