#pragma once

#include "CoreMinimal.h"
#include "DbTypeMap.h"
#include "Dump.h"

class FDbResultSet
{
public:
	typedef uint32 SizeType;
	typedef int32 DifferenceType;

	class FDbRow;
	class FDbField;
	class FConstRowIterator;

	typedef FConstRowIterator ConstIterator;
	// TODO: Reverse iterator

	typedef FDbRow ValueType;
	typedef const ValueType Ref;
	typedef ConstIterator Ptr;

	class FConstFieldIterator;
	// TODO: Reverse iterator

private:
	typedef const FDbResultSet* ResultPtr;

public:
	/** Not-a-position constant */
	// Was NPos
	static const SizeType InvalidPosition;

public:
	FDbResultSet();

	SizeType Num() const;
	bool IsEmpty() const;

	ConstIterator begin() const;
	ConstIterator end() const;

	/** Get the first row */
	Ref Front() const;

	/** Get the last row */
	Ref Last() const;

	Ref operator[](SizeType Index) const;

	Ref At(SizeType Index) const;

	operator bool() const
	{
		return !IsEmpty();
	}

	bool operator!() const
	{
		return *this;
	}

public:
	class FDbRow
	{
	public:
		typedef int16 SizeType;
		typedef int32 DifferenceType;

		typedef FConstRowIterator ConstIterator;
		//typedef FConstReverseFieldIterator ConstReverseIterator;

		typedef class FDbField ValueType;
		typedef class FDbField Ref;
		typedef ConstIterator Ptr;

		static const SizeType InvalidPosition;

	public:
		/** Index of the row in the result set */
		SizeType GetRowIndex() const { return RowIndex; }

		/** Number of fields in the row */
		SizeType Num() const;

		bool IsEmpty() const;

		ConstIterator begin() const;
		ConstIterator end() const;

		Ref operator[](SizeType ColumnIndex) const;
		Ref operator[](const FString& Name) const;

		template <typename... ElementType>
		void To(TTuple<ElementType...>&) const;

		template <typename... ElementType>
		void To(TTuple<ElementType&...>) const;

		template <typename... ElementType>
		void To(ElementType&...) const;

		template <typename... ElementType>
		void To(const ::std::initializer_list<FString>& Names, TTuple<ElementType...>&) const;

		template <typename... ElementType>
		void To(const ::std::initializer_list<FString>& Names, TTuple<ElementType&...>) const;

		template <typename... ElementType>
		void To(const ::std::initializer_list<FString>& Names, ElementType&... OutValues) const;

		FDbResultSet::SizeType IndexOfName(const FString& Name) const { return Result->GetIndexOfName(Name); }

	protected:
		friend class FDbResultSet;

		FDbRow(const ResultPtr InResult, const FDbResultSet::SizeType Index)
			: Result(InResult),
			RowIndex(Index) { }

		FDbResultSet::ResultPtr Result;
		FDbResultSet::SizeType RowIndex;
	};

	class FConstRowIterator
		: public DbPlus::Detail::TDataIterator<FConstRowIterator, FDbRow>
	{
		using Super = TDataIterator<FConstRowIterator, FDbRow>;

	public:
		FConstRowIterator() : Super(nullptr, InvalidPosition) { }

		int32 Compare(const FConstRowIterator& Other) const;
		FConstRowIterator& Advance(DifferenceType);
		bool IsValid() const { return Result && RowIndex <= Result->Num(); }

	private:
		friend class FDbResultSet;
		FConstRowIterator(FDbResultSet::ResultPtr InResult, FDbResultSet::SizeType InIndex) : Super(InResult, InIndex) { }
	};

public:
	class FDbField
	{
	public:
		FDbResultSet::SizeType GetRowIndex() const { return RowIndex; }
		FDbRow::SizeType GetFieldIndex() const { return FieldIndex; }

		const FString& GetName() const;
		const FDbFieldDescription& GetDescription() const;

		bool IsNull() const;
		bool IsEmpty() const;

		template <typename ElementType>
		bool To(ElementType& OutValue) const
		{
			return ToNullable(OutValue, Detail::Traits::TIsNullable<ElementType>());
		}

		template <typename ElementType>
		bool To(TOptional<ElementType>& OutValue) const
		{
			if (IsNull())
			{
				OutValue = TOptional<ElementType>();
				return true;
			}
			else
			{
				typename TDecay<ElementType> Temp;
				Detail::Traits::THasParser<ElementType, Detail::EProtocolDataFormat::PDF_Binary>());
				OutValue = TOptional<ElementType>(Temp);
				return true;
			}
			return false;
		}

		template <typename ElementType>
		typename TDecay<ElementType>::Type As() const
		{
			typename TDecay<ElementType>::Type Value;
			To(Value);
			return Value;
		}

		template <typename ElementType>
		typename TDecay<ElementType>::Type Coalesce(const ElementType& DefaultValue)
		{
			return IsNull() ? DefaultValue : As<ElementType>();
		}

	private:
		template <typename ElementType>
		bool ToNullable(ElementType& OutValue, const std::true_type&) const
		{
			typedef DbPlus::Detail::Traits::TNullableTraits<ElementType> NullableTraits;

			if (IsNull())
			{
				NullableTraits::SetNull(OutValue);
				return true;
			}

			return ToImplementation(OutValue, DbPlus::Detail::Traits::THasParser<ElementType, DbPlus::Detail::EProtocolDataFormat::PDF_Binary>());
		}

		template <typename ElementType>
		bool ToNullable(ElementType& OutValue, const std::false_type&) const
		{
			if (IsNull())
			{
				// TODO: Throw error that the value is null but it's not actually nullable
				return false;
			}

			return DbPlus::Detail::Traits::THasParser<ElementType, DbPlus::Detail::EProtocolDataFormat::PDF_Binary>();
		}

		template <typename ElementType>
		bool ToImplementation(ElementType& OutValue, const std::true_type&) const
		{
			const auto& FieldDescription = GetDescription();

			if (FieldDescription.GetFormatCode() == DbPlus::Detail::EProtocolDataFormat::PDF_Text)
				DbPlus::Detail::ProtocolRead< DbPlus::Detail::EProtocolDataFormat::PDF_Text>(Buffer.begin(), Buffer.end(), OutValue);
			else
				DbPlus::Detail::ProtocolRead< DbPlus::Detail::EProtocolDataFormat::PDF_Binary>(Buffer.begin(), Buffer.end(), OutValue);
			return true;
		}

		template <typename ElementType>
		bool ToImplementation(ElementType& OutValue, const std::false_type&) const
		{
			const auto& FieldDescription = GetDescription();
			if (FieldDescription.GetFormatCode() == DbPlus::Detail::EProtocolDataFormat::PDF_Binary)
			{
				// TODO: Throw error that the field has no parser
				return false;
			}

			auto Buffer = GetInputBuffer();
			DbPlus::Detail::ProtocolRead< DbPlus::Detail::EProtocolDataFormat::PDF_Text>(Buffer.begin(), Buffer.end(), OutValue);
			return true;
		}

		FDbFieldBuffer GetInputBuffer() const;

	protected:
		friend class FDbResultSet;
		friend class FDbRow;

		FDbField(ResultPtr InResult, SizeType InRow, FDbRow::SizeType InColumn)
			: Result(InResult),
			RowIndex(InRow),
			FieldIndex(InColumn) { }

		FDbResultSet::ResultPtr Result;
		FDbResultSet::SizeType RowIndex;
		FDbRow::SizeType FieldIndex;
	};

	class FConstFieldIterator
		: public DbPlus::Detail::TDataIterator<FConstFieldIterator, FDbField>
	{
		using Super = TDataIterator<FConstFieldIterator, FDbField>;

	public:
		FConstFieldIterator() : Super(nullptr, InvalidPosition, InvalidPosition) { }

		int32 Compare(const FConstFieldIterator& Other) const;
		FConstFieldIterator& Advance(DifferenceType Distance);
		bool IsValid() const { return Result && RowIndex <= Result->Num() && FieldIndex <= Result->GetColumnNum(); }

	private:
		friend class FDbRow;

		FConstFieldIterator(FDbResultSet::ResultPtr InResult, FDbResultSet::SizeType InRow, FDbResultSet::SizeType InColumn)
			: Super(InResult, InRow, InColumn) { }
	};

public:
	FDbRow::SizeType GetColumnNum() const;
	const DbPlus::Detail::FDbRowDescription& GetRowDescription() const;
	SizeType GetIndexOfName(const FString& Name) const;
	const DbPlus::Detail::FDbFieldDescription& GetFieldDescription(SizeType ColumnIndex) const;
	const DbPlus::Detail::FDbFieldDescription& GetFieldDescription(const FString& Name) const;
	const FString& GetFieldName(SizeType ColumnIndex) const;

private:
	friend class FDbRow;
	friend class FDbField;

	FDbFieldBuffer At(SizeType RowIndex, FDbRow::SizeType ColumnIndex) const;
	bool IsNull(SizeType RowIndex, FDbRow::SizeType ColumnIndex) const;
};

inline FDbResultSet::FDbRow::DifferenceType
operator-(const FDbResultSet::FDbRow& Left, const FDbResultSet::FDbRow& Right)
{
	return Left.GetRowIndex() - Right.GetRowIndex();
}
