// Copyright 2018 Jianzhao Fu. All Rights Reserved.
#include "DirectExcelLibrary.h"
#include "ExcelWorkbook.h"
#include "Misc/Paths.h"
#include "LogTypes.h"



FString UDirectExcelLibrary::ToAbsolutePath(FString projectReleativePath, bool isSave /*= false*/)
{
	FString baseDir = isSave ? FPaths::ProjectSavedDir() : FPaths::ProjectContentDir();
	FString fullPath = FPaths::Combine(baseDir, projectReleativePath);
	fullPath = FPaths::ConvertRelativePathToFull(fullPath);
	return fullPath;
}

bool UDirectExcelLibrary::DoesExcelFileExists(FString path, bool isAbsolutePath /*= true*/, bool isSave /*= false*/)
{
	if (!isAbsolutePath)
	{
		path = ToAbsolutePath(path, isSave);
	}
	return FPaths::FileExists(path);
}

UExcelWorkbook* UDirectExcelLibrary::LoadExcel(FString path, bool isAbsolutePath, bool isSave /*= false*/)
{
	UExcelWorkbook* wb = NewObject<UExcelWorkbook>();
	if (!isAbsolutePath)
	{
		path = ToAbsolutePath(path,isSave);
	}
	if (wb->Load(path))
	{
		return wb;
	}
	return nullptr;
}

bool UDirectExcelLibrary::SaveExcel(UExcelWorkbook* workbook, FString path, bool isAbsolutePath, bool isSave /*= false*/)
{
	if (!isAbsolutePath)
	{
		path = ToAbsolutePath(path,isSave);
	}

	return workbook->SaveAs(path, true);
}

UExcelWorkbook* UDirectExcelLibrary::CreateExcel()
{
	return NewObject<UExcelWorkbook>();
}
