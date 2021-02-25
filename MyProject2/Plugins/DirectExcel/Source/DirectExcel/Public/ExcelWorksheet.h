// Copyright 2018 Jianzhao Fu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty.h"

#include "ExcelCell.h"
#include "ExcelCellReference.h"
#include "ExcelCellRangeReference.h"

#include "ExcelWorksheet.generated.h"

/**
* Excel sheet struct
*/
UCLASS(Blueprintable, BlueprintType, Category = "DirectExcel")
class DIRECTEXCEL_API UExcelWorksheet :public UObject
{
	GENERATED_BODY()
public:
	void SetWorkbook(class UExcelWorkbook* val);

	xlnt::worksheet Data()const { return mData; }
	void SetData(xlnt::worksheet data) { mData = data; }
public:
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Workbook")
		UExcelWorkbook* ParentWorkbook()const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		bool IsValid() { return mData != nullptr; }

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FString Title()const;

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet", meta = (ToolTip = "Set title, cannot use *:/\\?[]"))
		void SetTitle(FString title);

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 Id()const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 Index()const;

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		FString Print(bool bPrintToScreen = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);

public:	//row
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 RowCount(bool skipNull = true)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 LowestRow()const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 HighestRow()const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void ClearRow(int32 rowIndex = 1);

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		FString PrintRow(int32 rowIndex = 1, bool bPrintToScreen = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);
public:	//column
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 ColumnCount(bool skipNull = true)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 LowestColumn()const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 HighestColumn()const;

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void ClearColumn(int32 columnIndex);

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void ClearColumnWithString(FString columnString);

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		FString PrintColumn(int32 columnIndex, bool bPrintToScreen = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		FString PrintColumnWithString(FString columnString, bool bPrintToScreen = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);

public://cells
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> CellsAtRow(int32 rowIndex = 1, ExcelSortType sortType = ExcelSortType::None)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> CellsAtColumn(int32 columnIndex, ExcelSortType sortType = ExcelSortType::None)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> CellsAtColumnString(FString columnString, ExcelSortType sortType = ExcelSortType::None)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FExcelCellRangeReference CalculateDimension()const;

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void MergeCells(FExcelCellRangeReference rangeReference);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void UnmergeCells(FExcelCellRangeReference rangeReference);
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FExcelCellRangeReference> AllMergedRanges();
public:	//cell
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		bool HasCell(FExcelCellReference cellReference)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		UExcelCell* CellAt(int32 column = 1, int32 row = 1)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		UExcelCell* CellAtRef(FExcelCellReference cellReference)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		UExcelCell* CellAtId(FString cellString)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void ClearCell(FExcelCellReference cellReference);

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		bool HasActiveCell()const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		UExcelCell* ActiveCell()const;
public:
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		bool ToBool(FExcelCellReference cellReference)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 ToInt(FExcelCellReference cellReference)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		float ToFloat(FExcelCellReference cellReference)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FString ToString(FExcelCellReference cellReference)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FDateTime ToDateTime(FExcelCellReference cellReference)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		bool ToBoolWithIndex(int32 column, int32 row)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 ToIntWithIndex(int32 column, int32 row)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		float ToFloatWithIndex(int32 column, int32 row)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FString ToStringWithIndex(int32 column, int32 row)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FDateTime ToDateTimeWithIndex(int32 column, int32 row)const;


	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		bool ToBoolWithId(FString cellReferenceString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		int32 ToIntWithId(FString cellReferenceString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		float ToFloatWithId(FString cellReferenceString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FString ToStringWithId(FString cellReferenceString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		FDateTime ToDateTimeWithId(FString cellReferenceString)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<bool> BoolArrayAtRow(int32 rowIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<int32> IntArrayAtRow(int32 rowIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<float> FloatArrayAtRow(int32 rowIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FString> StringArrayAtRow(int32 rowIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FDateTime> DateTimeArrayAtRow(int32 rowIndex)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<bool> BoolArrayAtColumn(int32 columnIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<int32> IntArrayAtColumn(int32 columnIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<float> FloatArrayAtColumn(int32 columnIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FString> StringArrayAtColumn(int32 columnIndex)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FDateTime> DateTimeArrayAtColumn(int32 columnIndex)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<bool> BoolArrayAtColumnString(FString columnString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<int32> IntArrayAtColumnString(FString columnString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<float> FloatArrayAtColumnString(FString columnString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FString> StringArrayAtColumnString(FString columnString)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FDateTime> DateTimeArrayAtColumnString(FString columnString)const;


	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<bool> BoolArrayAtRowSorted(int32 rowIndex, TArray<int32>& outColumnIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<int32> IntArrayAtRowSorted(int32 rowIndex, TArray<int32>& outColumnIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<float> FloatArrayAtRowSorted(int32 rowIndex, TArray<int32>& outColumnIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FString> StringArrayAtRowSorted(int32 rowIndex, TArray<int32>& outColumnIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FDateTime> DateTimeArrayAtRowSorted(int32 rowIndex, TArray<int32>& outColumnIndices, ExcelSortType sortType = ExcelSortType::None)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<bool> BoolArrayAtColumnSorted(int32 columnIndex, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<int32> IntArrayAtColumnSorted(int32 columnIndex, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<float> FloatArrayAtColumnSorted(int32 columnIndex, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FString> StringArrayAtColumnSorted(int32 columnIndex, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FDateTime> DateTimeArrayAtColumnSorted(int32 columnIndex, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;

	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<bool> BoolArrayAtColumnStringSorted(FString columnString, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<int32> IntArrayAtColumnStringSorted(FString columnString, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<float> FloatArrayAtColumnStringSorted(FString columnString, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FString> StringArrayAtColumnStringSorted(FString columnString, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
	UFUNCTION(BlueprintPure, Category = "DirectExcel|Worksheet")
		TArray<FDateTime> DateTimeArrayAtColumnStringSorted(FString columnString, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const;
public:
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetBool(FExcelCellReference cellReference, bool value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetInt(FExcelCellReference cellReference, int32 value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetFloat(FExcelCellReference cellReference, float value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetString(FExcelCellReference cellReference, FString value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetDateTime(FExcelCellReference cellReference, FDateTime value);

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetBoolWithIndex(int32 column, int32 row, bool value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetIntWithIndex(int32 column, int32 row, int32 value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetFloatWithIndex(int32 column, int32 row, float value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetStringWithIndex(int32 column, int32 row, FString value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetDateTimeWithIndex(int32 column, int32 row, FDateTime value);


	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetBoolWithId(FString cellReferenceString, bool value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetIntWithId(FString cellReferenceString, int32 value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetFloatWithId(FString cellReferenceString, float value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetStringWithId(FString cellReferenceString, FString value);
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		void SetDateTimeWithId(FString cellReferenceString, FDateTime value);
public://find
	//return row index
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindBoolAtColumn(int32 columnIndex, bool searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindIntAtColumn(int32 columnIndex, int32 searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindFloatAtColumn(int32 columnIndex, float searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindStringAtColumn(int32 columnIndex, FString searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindDateTimeAtColumn(int32 columnIndex, FDateTime searchValue, int32 startRowIndex = 1)const;

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindBoolAtColumnString(FString columnString, bool searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindIntAtColumnString(FString columnString, int32 searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindFloatAtColumnString(FString columnString, float searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindStringAtColumnString(FString columnString, FString searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindDateTimeAtColumnString(FString columnString, FDateTime searchValue, int32 startRowIndex = 1)const;

	//return column index
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindBoolAtRow(int32 rowIndex, bool searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindIntAtRow(int32 rowIndex, int32 searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindFloatAtRow(int32 rowIndex, float searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindStringAtRow(int32 rowIndex, FString searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		UExcelCell* FindDateTimeAtRow(int32 rowIndex, FDateTime searchValue, int32 startColumnIndex = 1)const;

public://find multiple
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleBoolAtColumn(int32 columnIndex, bool searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleIntAtColumn(int32 columnIndex, int32 searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleFloatAtColumn(int32 columnIndex, float searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleStringAtColumn(int32 columnIndex, FString searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleDateTimeAtColumn(int32 columnIndex, FDateTime searchValue, int32 startRowIndex = 1)const;

	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleBoolAtColumnString(FString columnString, bool searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleIntAtColumnString(FString columnString, int32 searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleFloatAtColumnString(FString columnString, float searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleStringAtColumnString(FString columnString, FString searchValue, int32 startRowIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleDateTimeAtColumnString(FString columnString, FDateTime searchValue, int32 startRowIndex = 1)const;

	//return column index
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleBoolAtRow(int32 rowIndex, bool searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleIntAtRow(int32 rowIndex, int32 searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleFloatAtRow(int32 rowIndex, float searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleStringAtRow(int32 rowIndex, FString searchValue, int32 startColumnIndex = 1)const;
	UFUNCTION(BlueprintCallable, Category = "DirectExcel|Worksheet")
		TArray<UExcelCell*> FindMultipleDateTimeAtRow(int32 rowIndex, FDateTime searchValue, int32 startColumnIndex = 1)const;
private:
	template<typename T>
	TArray<T> Internal_ArrayAtColumnStringSorted(FString columnString, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const
	{
		int32 columnIndex = UExcelCellReferenceLibrary::ToColumnIndex(columnString);
		return Internal_ArrayAtColumnSorted<T>(columnIndex, outRowIndices, sortType);
	}

	template<typename T>
	TArray<T> Internal_ArrayAtColumnSorted(int32 columnIndex, TArray<int32>& outRowIndices, ExcelSortType sortType = ExcelSortType::None)const
	{
		TArray<UExcelCell*> cells = CellsAtColumn(columnIndex, sortType);
		TArray<T> result;
		for (int32 i = 0; i < cells.Num(); ++i)
		{
			UExcelCell* cell = cells[i];
			result.Add(DirectExcel::ToValue<T>(cell));
			outRowIndices.Add(cell->Row());
		}
		return result;
	}
	template<typename T>
	TArray<T> Internal_ArrayAtRowSorted(int32 rowIndex, TArray<int32>& outColumnIndices, ExcelSortType sortType = ExcelSortType::None)const
	{
		TArray<UExcelCell*> cells = CellsAtRow(rowIndex, sortType);
		TArray<T> result;
		for (int32 i = 0; i < cells.Num(); ++i)
		{
			UExcelCell* cell = cells[i];
			result.Add(DirectExcel::ToValue<T>(cell));
			outColumnIndices.Add(cell->Column());
		}
		return result;
	}

	template<typename T>
	int32 FindRowAtColumn(int32 columnIndex, T searchValue)const
	{
		UExcelCell* cell = FindCellAtColumn(columnIndex, searchValue);
		return cell != nullptr ? cell->Row() : -1;
	}

	template<typename T>
	int32 FindColumnAtRow(int32 rowIndex, T searchValue)const
	{
		UExcelCell* cell = FindCellAtRow(rowIndex, searchValue);
		return cell != nullptr ? cell->Column() : -1;
	}

	template<typename T>
	UExcelCell* FindCellAtColumn(int32 columnIndex, T searchValue, int32 startRowIndex = 1)const
	{
		TArray<UExcelCell*> cells = CellsAtColumn(columnIndex);
		startRowIndex = FMath::Clamp(startRowIndex - 1, 0, cells.Num() - 1);
		for (int32 i = startRowIndex; i < cells.Num(); ++i)
		{
			UExcelCell* cell = cells[i];
			if (DirectExcel::ToValue<T>(cell) == searchValue)
			{
				return cell;
			}
		}
		return nullptr;
	}

	template<typename T>
	UExcelCell* FindCellAtRow(int32 rowIndex, T searchValue, int32 startColumnIndex = 1)const
	{
		TArray<UExcelCell*> cells = CellsAtRow(rowIndex);
		startColumnIndex = FMath::Clamp(startColumnIndex - 1, 0, cells.Num() - 1);
		for (int32 i = startColumnIndex; i < cells.Num(); ++i)
		{
			UExcelCell* cell = cells[i];
			if (DirectExcel::ToValue<T>(cell) == searchValue)
			{
				return cell;
			}
		}
		return nullptr;
	}

	template<typename T>
	TArray<UExcelCell*> FindMultipleCellAtColumn(int32 columnIndex, T searchValue, int32 startRowIndex = 1)const
	{
		TArray<UExcelCell*> results;
		TArray<UExcelCell*> cells = CellsAtColumn(columnIndex);
		startRowIndex = FMath::Clamp(startRowIndex - 1, 0, cells.Num() - 1);
		for (int32 i = startRowIndex; i < cells.Num(); ++i)
		{
			UExcelCell* cell = cells[i];
			if (DirectExcel::ToValue<T>(cell) == searchValue)
			{
				results.Add(cell);
			}
		}
		return results;
	}
	template<typename T>
	TArray<UExcelCell*> FindMultipleCellAtRow(int32 rowIndex, T searchValue, int32 startColumnIndex = 1)const
	{
		TArray<UExcelCell*> results;
		TArray<UExcelCell*> cells = CellsAtRow(rowIndex);
		startColumnIndex = FMath::Clamp(startColumnIndex - 1, 0, cells.Num() - 1);
		for (int32 i = startColumnIndex; i < cells.Num(); ++i)
		{
			UExcelCell* cell = cells[i];
			if (DirectExcel::ToValue<T>(cell) == searchValue)
			{
				results.Add(cell);
			}
		}
		return results;
	}
private:
	UExcelCell* CreateCell(xlnt::cell c) const;
private:
	TWeakObjectPtr<class UExcelWorkbook> mWorkbook;
	xlnt::worksheet mData;
};
