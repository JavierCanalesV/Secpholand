// Fill out your copyright notice in the Description page of Project Settings.

#include "RWTxFile.h"

bool URWTxFile::LoadTxt(FString FileNameA, FString& SaveTextA)
{
	return FFileHelper::LoadFileToString(SaveTextA, *(FPaths::ProjectContentDir() + FileNameA));
}

bool URWTxFile::SaveTxt(FString SaveTextB, FString FileNameB)
{
	return FFileHelper::SaveStringToFile(SaveTextB, *(FPaths::ProjectContentDir() + FileNameB));
}