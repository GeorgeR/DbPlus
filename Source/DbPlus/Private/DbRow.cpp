#include "DbResultSet.h"

#include <limits>

const FDbResultSet::FDbRow::SizeType FDbResultSet::FDbRow::InvalidPosition = std::numeric_limits<FDbResultSet::FDbRow::SizeType>::max();

FDbResultSet::FDbRow::SizeType FDbResultSet::FDbRow::Num() const
{
	return Result->GetColumnNum();
}

bool FDbResultSet::FDbRow::IsEmpty() const
{
	return Num() == 0;
}

FDbResultSet::FDbRow::ConstIterator FDbResultSet::FDbRow::begin() const
{
	return ConstIterator(Result, RowIndex, 0);
}

FDbResultSet::FDbRow::ConstIterator FDbResultSet::FDbRow::end() const
{
	return ConstIterator(Result, RowIndex, Num());
}

FDbResultSet::FDbRow::Ref FDbResultSet::FDbRow::operator[](SizeType ColumnIndex) const
{
	return Ref(Result, RowIndex, ColumnIndex);
}

FDbResultSet::FDbRow::Ref FDbResultSet::FDbRow::operator[](const FString& Name) const
{
    const auto ColumnIndex = Result->GetIndexOfName(Name);
	return (*this)[ColumnIndex];
}

int32 FDbResultSet::FConstRowIterator::Compare(const FConstRowIterator& Other) const
{
	if (!(*this) && !Other)
		return 0;

	ensureMsgf(Result == Other.Result, TEXT("Cannot compare iterators in different result sets"));

	if (RowIndex != Other.RowIndex)
		return (RowIndex <= Other.RowIndex) ? -1 : 1;

	return 0;
}

FDbResultSet::FConstRowIterator& FDbResultSet::FConstRowIterator::Advance(DifferenceType Distance)
{
    if(*this)
    {
		auto Target = Distance + RowIndex;
		if (Target < 0 || StaticCast<FDbResultSet::SizeType>(Target) > Result->Num())
			RowIndex = InvalidPosition;
		else
			RowIndex = Target;
    }
	else if(Result)
	{
		if (Distance == 1)
			RowIndex = 0;
		else if (Distance == -1)
			RowIndex = Result->Num() - 1;
	}

	return *this;
}

template <typename ... ElementType>
void FDbResultSet::FDbRow::To(TTuple<ElementType...>&) const
{
}

template <typename ... ElementType>
void FDbResultSet::FDbRow::To(TTuple<ElementType&...>) const
{
}

template <typename ... ElementType>
void FDbResultSet::FDbRow::To(ElementType&...) const
{
}

template <typename ... ElementType>
void FDbResultSet::FDbRow::To(const ::std::initializer_list<FString>& Names,
	TTuple<ElementType...>&) const
{
}

template <typename ... ElementType>
void FDbResultSet::FDbRow::To(const ::std::initializer_list<FString>& Names,
	TTuple<ElementType&...>) const
{
}

template <typename ... ElementType>
void FDbResultSet::FDbRow::To(const ::std::initializer_list<FString>& Names,
	ElementType&... OutValues) const
{
}


