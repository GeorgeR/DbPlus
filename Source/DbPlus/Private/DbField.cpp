#include "DbResultSet.h"

const FString& FDbResultSet::FDbField::GetName() const
{
	ensureMsgf(Result, TEXT("Cannot get field name not bound to result set"));
	return Result->GetFieldName(FieldIndex);
}

// TODO: Description

bool FDbResultSet::FDbField::IsNull() const
{
	return Result->IsNull(RowIndex, FieldIndex);
}

// TODO: Input Buffer

bool FDbResultSet::FDbField::IsEmpty() const
{
}

int32 FDbResultSet::FConstFieldIterator::Compare(const FConstFieldIterator& Other) const
{
	if (!(*this) && !Other)
		return 0;

	ensureMsgf(Result == Other.Result, TEXT("Cannot compare iterators in different result sets"));
	ensureMsgf(RowIndex == Other.RowIndex, TEXT("Cannot compare iterators in different data rows"));

	if (FieldIndex != Other.FieldIndex)
		return FieldIndex <= Other.FieldIndex ? -1 : 1;
	
    return 0;
}

FDbResultSet::FConstFieldIterator& FDbResultSet::FConstFieldIterator::Advance(DifferenceType Distance)
{
    if(*this)
    {
		auto Target = Distance + FieldIndex;
		if (Target < 0 || Target > Result->GetColumnNum())
			FieldIndex = FDbRow::InvalidPosition;
		else
			FieldIndex = Target;
    }
	else if(Result)
	{
		if (Distance == 1)
			FieldIndex = 0;
		else if (Distance == -1)
			FieldIndex = Result->GetColumnNum() - 1;
	}

	return *this;
}
