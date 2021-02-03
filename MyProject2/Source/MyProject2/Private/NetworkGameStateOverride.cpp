// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameStateOverride.h"

void ANetworkGameStateOverride::GetUserCpp(UPARAM(ref) TArray<FUserInfocpp> & userStruct, const int _ID, const FString Username, const FString UniqueID, const APlayerController* PlayerController, const TEnumAsByte<EUserSearchType> LookBye, bool & _found, struct FUserInfocpp & foundUser, int & Index)
{
	for (int i = 0; i < userStruct.Num(); i++) {

		foundUser = userStruct[i];
		Index = i; 
		switch (LookBye)
		{
		case EUserSearchType::ID:
			if (userStruct[i].ID == _ID) {
				_found = true;
				return;
			}
			break;
		case EUserSearchType::UserName:
			if (userStruct[i].UserName == Username) {
				_found = true;
				return;
			}
			break;
		case EUserSearchType::PlayerController:
			if (userStruct[i].PlayerController == PlayerController) {
				_found = true;
				return;
			}
			break;
		case EUserSearchType::UniqueID:
			if (userStruct[i].UniqueID == UniqueID) {
				_found = true;
				return;
			}
			break;
		default:
			break;
		}
		
	}

	return;
}