// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSideControllerOverride.h"
#include "Engine/World.h"

FString AClientSideControllerOverride::GetPlayerPort() const
{
	return GetWorld()->GetAddressURL();
}

