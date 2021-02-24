// Copyright 2018 Jianzhao Fu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DirectExcelLibrary.generated.h"
/**
* Helper functions to load/save excel file
*/
UCLASS(DisplayName = "DirectExcel", Category = "DirectExcel")
class DIRECTEXCEL_API UDirectExcelLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "DirectExcel")
		static class UExcelWorkbook* LoadExcel(FString path, bool isAbsolutePath = true, bool isSave = false);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel")
		static bool SaveExcel(UExcelWorkbook* workbook, FString path, bool isAbsolutePath = true, bool isSave = false);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel")
		static class UExcelWorkbook* CreateExcel();
	UFUNCTION(BlueprintPure, Category = "DirectExcel")
		static FString ToAbsolutePath(FString projectReleativePath, bool isSave = false);
	UFUNCTION(BlueprintPure, Category = "DirectExcel")
		static bool DoesExcelFileExists(FString path, bool isAbsolutePath = true, bool isSave = false);
};
