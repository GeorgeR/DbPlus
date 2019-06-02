#include "DbResultSet.h"

#include <limits>

const FDbResultSet::SizeType FDbResultSet::InvalidPosition = std::numeric_limits<FDbResultSet::SizeType>::max();

FDbResultSet::FDbResultSet()
{
}

FDbResultSet::SizeType FDbResultSet::Num() const
{
	return GetColumnNum();
}

bool FDbResultSet::IsEmpty() const
{
	return Num() == 0;
}

FDbResultSet::ConstIterator FDbResultSet::begin() const
{
	return ConstIterator(this, 0);
}

FDbResultSet::ConstIterator FDbResultSet::end() const
{
	return ConstIterator(this, Num());
}

FDbResultSet::Ref FDbResultSet::Front() const
{
	ensureMsgf(Num() > 0, TEXT("Cannot get row in an empty result set"));
	return FDbRow(this, 0);
}

FDbResultSet::Ref FDbResultSet::Last() const
{
	ensureMsgf(Num() > 0, TEXT("Cannot get row in an empty result set"));
	return FDbRow(this, Num() - 1);
}

FDbResultSet::Ref FDbResultSet::operator[](SizeType Index) const
{
	ensureMsgf(Index < Num(), TEXT("Index is out of bounds"));
	return FDbRow(this, Index);
}

FDbResultSet::FDbRow::SizeType FDbResultSet::GetColumnNum() const
{
	return GetRowDescription().Num();
}

const DbPlus::Detail::FDbRowDescription& FDbResultSet::GetRowDescription() const
{
}

FDbResultSet::SizeType FDbResultSet::GetIndexOfName(const FString& Name) const
{
	const auto& Description = GetRowDescription();
    const auto FieldIndex = Description.IndexOfByPredicate([Name](const DbPlus::Detail::FDbFieldDescription& FieldDescription)
    {
		return FieldDescription.Name == Name;
	});

	return FieldIndex == INDEX_NONE ? InvalidPosition : FieldIndex;
}

const DbPlus::Detail::FDbFieldDescription& FDbResultSet::GetFieldDescription(const SizeType ColumnIndex) const
{
	return GetRowDescription()[ColumnIndex];
}

const DbPlus::Detail::FDbFieldDescription& FDbResultSet::GetFieldDescription(const FString& Name) const
{
	for (const auto& FieldDescription : GetRowDescription())
		if (FieldDescription.Name == Name)
			return FieldDescription;

    // TODO: throw not found error
    
}

const FString& FDbResultSet::GetFieldName(const SizeType ColumnIndex) const
{
	return GetFieldDescription(ColumnIndex).Name;
}

FDbResultSet::Ref FDbResultSet::At(SizeType Index) const
{
	CheckRowIndex(RowIndex);
	const auto& RowData = Rows[RowIndex];
	return RowData.GetFieldData(ColumnIndex);
}

bool FDbResultSet::IsNull(SizeType RowIndex, FDbRow::SizeType ColumnIndex) const
{
	CheckRowIndex(RowIndex);
	const auto& RowData = Rows[RowIndex];
	return RowData.IsNull(ColumnIndex);
}
