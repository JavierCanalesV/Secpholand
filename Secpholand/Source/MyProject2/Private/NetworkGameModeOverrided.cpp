// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameModeOverrided.h"
#include "Engine/World.h"

void ANetworkGameModeOverrided::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	AGameModeBase::PreLogin(Options, Address, UniqueId, ErrorMessage);
	playerAddress = Address;
	playerUniqueID = UniqueId.ToString();
}

FString ANetworkGameModeOverrided::GetPlayerIP() const
{
	return playerAddress;
}

FString ANetworkGameModeOverrided::GetPlayerUniqueID() const
{
	return playerUniqueID;
}

FString ANetworkGameModeOverrided::GetPlayerPort() const
{
	return GetWorld()->GetAddressURL();
}
