#pragma once

#include "CoreMinimal.h"

namespace DbPlus
{
    namespace Detail
    {
		template <typename T>
		using TNullable = TOptional<T>;

		enum class EProtocolDataFormat
		{
			PDF_Unknown = 0,
			PDF_Text = 1,
			PDF_Binary = 2
		};

		struct FDbFieldDescription
		{
		public:
			/** The field name */
			FString Name;

			/** If the field is a column of a table, has the table's object id, otherwise 0. */
			int32 TableObjectId;

			/** If the field is a column of a table, has the attribute number of the column, otherwise 0. */
			int16 AttributeNumber;

			/** The Object ID of the field's data type. */
			FObjectIdType TypeObjectId;

			/** The data type size. Negative values denote variable-width types. */
			int16 TypeSize;

			/** The type modifier. */
			int32 TypeModifier;

			/** Text or Binary? */
			EProtocolDataFormat FormatCode;

			int32 MaxSize;
		};

		typedef TArray<FDbFieldDescription> FDbRowDescription;
		
		enum class EProtocolBinaryType
		{
			PBT_Other,
			PBT_Integral,
			PBT_FloatingPoint
		};

		typedef std::integral_constant<EProtocolBinaryType, EProtocolBinaryType::PBT_Other> OtherBinaryType;
		typedef std::integral_constant<EProtocolBinaryType, EProtocolBinaryType::PBT_Integral> IntegralBinaryType;
		typedef std::integral_constant<EProtocolBinaryType, EProtocolBinaryType::PBT_FloatingPoint> FloatingPointBinaryType;

		template <typename T> 
        struct TProtocolBinarySelector : OtherBinaryType { };

		template <> struct TProtocolBinarySelector<int8> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<uint8> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<int16> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<uint16> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<int32> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<uint32> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<int64> : IntegralBinaryType { };
		template <> struct TProtocolBinarySelector<uint64> : IntegralBinaryType { };

		template <> struct TProtocolBinarySelector<FFloat16> : FloatingPointBinaryType { };
		template <> struct TProtocolBinarySelector<float> : FloatingPointBinaryType { };
		template <> struct TProtocolBinarySelector<double> : FloatingPointBinaryType { };

        namespace Traits
        {
			template <typename T, ProtocolDataFormat Format>
			struct THasParser : std::false_type { };

			template <typename T, ProtocolDataFormat Format>
			struct THasFormatter : std::false_type { };

			template <typename T>
			struct TIsNullable : std::false_type { };

			template <typename T>
			struct TIsNullable<TOptional<T>> : std::true_type { };

			template <typename T>
			struct TNullableTraits
			{
				static bool IsNull(const T& Value) { return false; }
				static void SetNull(T& Value) { }
			};

			template <typename T>
			struct TNullableTraits<TOptional<T>>
			{
				typedef TOptional<T> ValueType;

				static bool IsNull(const T& Value) { return !Value.IsSet(); }
				static void SetNull(T& Value) { ValueType().Emplace(Value); }
			};
        }

		template <typename ElementType, ProtocolDataFormat Format>
		struct TProtocolParser;

		template <typename ElementType, ProtocolDataFormat Format>
		struct TProtocolFormatter;

		template <typename T, ProtocolDataFormat Format>
		struct TProtocolIOTraits
		{
			typedef InputIteratorBuffer InputBufferType;
			typedef TProtocolParser<T, Format> ParserType;
			typedef TProtocolFormatter<T, Format> FormatterType;
		};

		template <ProtocolDataFormat Format, typename ElementType>
		typename TProtocolIOTraits<ElementType, Format>::ParserType
		ProtocolReader(ElementType& Value)
		{
			return typename TProtocolIOTraits<ElementType, Format>::ParserType(Value);
		}

		template <ProtocolDataFormat Format, typename ElementType, typename InputIterator>
		InputIterator
		ProtocolRead(InputIterator Begin, InputIterator End, ElementType& Value)
		{
			static_assert(Traits::THasParser<ElementType, Format>::Value == true, TEXT("Type doesn't have an appropriate parser"));
			return typename TProtocolIOTraits<ElementType, Format>::ParserType(Value)(Begin, End);
		}

		template <ProtocolDataFormat Format, typename ElementType>
		typename TProtocolIOTraits<ElementType, Format>::FormatterType
		ProtocolWriter(const ElementType& Value)
		{
			return typename TProtocolIOTraits<ElementType, Format>::FormatterType(Value);
		}

		template <ProtocolDataFormat Format, typename ElementType>
		bool
			ProtocolWrite(TArray<uint8>& Buffer, const ElementType& Value)
		{
			static_assert(Traits::THasFormatter<ElementType, Format>::Value == true, TEXT("Type doesn't have an appropriate formatter"));
			return ProtocolWriter<Format>(Value)(Buffer);
		}

		template <ProtocolDataFormat Format, typename ElementType, typename OutputIterator>
		bool
		ProtocolWrite(OutputIterator Out, const ElementType& Value)
		{
			static_assert(Traits::THasFormatter<ElementType, Format>::Value == true, TEXT("Type doesn't have an appropriate formatter"));
			return ProtocolWriter<Format>(Value)(Out);
		}

        template <typename ElementType>
		struct TParserBase
		{
			typedef typename TDecay<ElementType>::Type ValueType;
			ValueType& Value;

            explicit TParserBase(ValueType& InValue) : Value(InValue) { }
		};

        template <typename ElementType>
        struct TFormatterBase
        {
			typedef typename TDecay<ElementType>::Type ValueType;
			const ValueType& Value;

            explicit TFormatterBase(const ValueType& InValue) : Value(InValue) { }
        };

        template <typename ElementType, EProtocolBinaryType Type>
		struct TBinaryDataParser;

        template <typename ElementType>
        struct TBinaryDataParser<ElementType, EProtocolBinaryType::PBT_Integral>
			: TParserBase<ElementType>
        {
			typedef TParserBase<ElementType> Super;
			typedef typename Super::ValueType ValueType;

            static size_t Size() { return sizeof(ElementType); }

            explicit TBinaryDataParser(ValueType& InValue) : Super(InValue) { }

            template <typename InputIterator>
			InputIterator
			operator()(InputIterator Begin, InputIterator End);
        };

        template <typename ElementType>
		struct TBinaryDataParser<ElementType, EProtocolBinaryType::PBT_Other>;

        template <typename ElementType, EProtocolBinaryType Type>
		struct TBinaryDataFormatter;

        template <typename ElementType>
        struct TBinaryDataFormatter<ElementType, EProtocolBinaryType::PBT_Integral>
			: TFormatterBase<ElementType>
        {
			typedef TFormatterBase<ElementType> Super;
			typedef typename Super::ValueType ValueType;

            static size_t Size() { return sizeof(ElementType); }

            explicit TBinaryDataFormatter(const ValueType& InValue) : Super(InValue) { }

			bool operator()(TArray<uint8>& Buffer);

            template <typename OutputIterator>
            bool operator()(OutputIterator);
        };

		//template <typename TypeOid, typename ElementType>
		//struct TDataMappingBase
		//{
		//	static constexpr what = TypeOid;
		//	using Type = typename TDecay<ElementType>::Type;
		//};

		namespace Traits
		{
			void RegisterBinaryParser();
			bool HasBinaryParser();

			template <typename T>
			struct THasInputOperator
			{
			private:
				static std::false_type Test(std::false_type);
				static std::true_type Test(std::istream&);

				static std::istream& Stream;
				static T& ValueRef;

			public:
				static constexpr bool Value = TIsSame<decltype(Test(Stream >> ValueRef)), std::true_type>::Value;
			};

			template <typename T>
			struct THasOutputOperator
			{
			private:
				static std::false_type Test(std::false_type);
				static std::true_type Test(std::ostream&);

				static std::ostream& Stream;
				static const T& ValueRef;

			public:
				static constexpr bool Value = TIsSame<decltype(Test(Stream << ValueRef)), std::true_type>::Value;
			};

			template <typename T>
			struct THasParser<T, TEXT_DATA_FORMAT> : std::integral_constant<bool, THasInputOperator<T>::Value> { };

			
		}

        template <typename FinalType, typename DataType>
		class TDataIterator
			: public DataType
        {
        public:
			using IteratorType = FinalType;

			using ValueType = DataType;
			using DifferenceType = int32;
			using Reference = ValueType;
			using Pointer = const ValueType*;
			using IteratorCategory = std::random_access_iterator_tag;

        public:
			Reference operator*() const { return Reference(*this); }
			Pointer operator->() const { return this; }

			operator bool() const { return Rebind().IsValid(); }
			bool operator!() const { return !Rebind().IsValid(); }

			IteratorType& operator++() { return Advance(1); }
            
            IteratorType operator++(int32)
			{
				IteratorType Previous{ Rebind() };
				Advance(1);
				return Previous;
			}

			IteratorType& operator--() { return Advance(-1); }
            
            IteratorType operator--(int32)
			{
				IteratorType{ Rebind() };
				Advance(-1);
				return Previous;
			}

			IteratorType& operator+=(DifferenceType Distance) { return Advance(Distance); }
			IteratorType& operator-=(DifferenceType Distance) { return Advance(-Distance); }

            bool operator==(const IteratorType& Other) const
			{
				return Compare(Other) == 0;
			}

            bool operator!=(const IteratorType& Other) const
			{
				return !(*this == Other);
			}

            bool operator<(const IteratorType& Other) const
			{
				return Compare(Other) < 0;
			}

            bool operator<=(const IteratorType& Other) const
			{
				return Compare(Other) <= 0;
			}

            bool operator>(const IteratorType& Other) const
			{
				return Compare(Other) > 0;
			}

            bool operator>=(const IteratorType& Other) const
			{
				return Compare(Other) >= 0;
			}

        protected:
            template <typename... ElementType>
            TDataIterator(ElementType... Args)
				: ValueType(Forward<ElementType>(Args)...) { }

        private:
			IteratorType& Rebind() { return StaticCast<IteratorType&>(*this); }
			const IteratorType& Rebind() const { return StaticCast<const IteratorType&>(*this); }
            
			IteratorType& Advance(DifferenceType Distance) { return Rebind().Advance(Distance); }

			int32 Compare(const IteratorType& Other) const { return Rebind().Compare(Other); }
        };
    }

	namespace Detail
	{
		template <size_t... Indexes>
		struct TIndexesTuple
		{
			static constexpr size_t Num = sizeof... (Indexes);
		};

		template <size_t Num, typename Tuple = TIndexesTuple<>>
		struct TIndexBuilder;

		template <size_t Num, size_t... Indexes>
		struct TIndexBuilder<Num, TIndexesTuple<Indexes...>>
			: TIndexBuilder<Num - 1, TIndexesTuple<Indexes..., sizeof... (Indexes)>> { };

		template <size_t... Indexes>
		struct TIndexBuilder<0, TIndexesTuple<Indexes...>>
		{
			// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
			typedef TIndexesTuple<Indexes...> Type;
			static constexpr size_t Num = sizeof... (Indexes);
		};

		template <size_t Index, typename ElementType>
		struct TNthField
		{
			enum { Index = Index };
			typedef ElementType FElementType;

            explicit TNthField(const FDbResultSet::Row& InRow) : Row(InRow) { }

			ElementType Value() { return Row[Index].template As<ElementType>(); }

			bool To(ElementType& OutValue) { return Row[Index].To(OutValue); }

			FDbResultSet::Row Row;
		};

		template <typename IndexTuple, typename... ElementType>
		struct TRowDataExtractorBase;

		template <size_t... Indexes, typename... ElementType>
		struct TRowDataExtractorBase<TIndexesTuple<Indexes...>, ElementType...>
		{
			static constexpr size_t Num = sizeof... (ElementType);

			static void GetTuple(const FDbResultSet::Row& Row, TTuple<ElementType...>& OutValue)
			{
				TTuple<ElementType...> Temp(TNthField<Indexes, ElementType>(Row).Value()...);
				Swap(Temp, OutValue);
			}

			static void GetValues(const FDbResultSet::Row& Row, ElementType&... OutValues)
			{
				Expand(TNthField<Indexes, ElementType>(Row).To(OutValues)...);
			}
		};

		template <typename... ElementType>
		struct TRowDataExtractor : TRowDataExtractorBase<typename TIndexBuilder<sizeof...(ElementType)>::Type, ElementType...> { };

		template <typename IndexTuple, typename... ElementType>
		struct TFieldByNameExtractor;

		template <size_t... Indexes, typename... ElementType>
		struct TFieldByNameExtractor<TIndexesTuple<Indexes...>, ElementType...>
		{
			static constexpr size_t Num = sizeof... (ElementType);

			static void GetTuple(const FDbResultSet::Row& Row, const std::initializer_list<FString>& Names, ElementType&... OutValue)
			{
				ensureMsgf(Names.size() < Num, TEXT("Not enough names in row data extraction"));

				TTuple<ElementType...> Temp(Row[*(Names.begin() + Indexes)].template As<ElementType>()...);
				Swap(Temp, OutValue);
			}

			static void GetValues(const FDbResultSet::Row& Row, const std::initializer_list<FString>& Names, ElementType&... OutValues)
			{
				Expand{ Row[*(Names.begin() + Indexes)].To(OutValues)... };
			}
		};

		template <typename... ElementType>
		struct TRowDataByNameExtractor
			: TFieldByNameExtractor<typename TIndexBuilder<sizeof... (ElementType)>::Type, ElementType...> { };
	}

	template <typename... ElementType>
	void FDbResultSet::Row::To(TTuple<ElementType...>& OutValue) const
	{
		Detail::TRowDataExtractor<ElementType...>::GetTuple(*this, OutValue);
	}

	template <typename... ElementType>
	void FDbResultSet::Row::To(ElementType&... OutValues) const
	{
		Detail::TRowDataExtractor<ElementType...>::GetValues(*this, OutValues...);
	}

	template <typename... ElementType>
	void FDbResultSet::Row::To(const std::initializer_list<FString>& Names, TTuple<ElementType...>& OutValue) const
	{
		Detail::TRowDataByNameExtractor<ElementType...>::GetTuple(*this, Names, OutValue);
	}

	template <typename... ElementType>
	void FDbResultSet::Row::To(const std::initializer_list<FString>& Names, TTuple<ElementType&...> OutValue) const
	{
		TTuple<ElementType...> NonRef;
		Detail::TRowDataByNameExtractor<ElementType...>::GetTuple(*this, Names, NonRef);
		OutValue = NonRef;
	}

	template <typename... ElementType>
	void FDbResultSet::Row::To(const std::initializer_list<FString>& Names, ElementType&... OutValues) const
	{
		Detail::TRowDataByNameExtractor<ElementType...>::GetValues(*this, Names, OutValues...);
	}
}

namespace DbPlus
{
    namespace Pg
    {
		namespace Detail
		{
			using FSmallInt = int16;
			using FUSmallInt = uint16;
			using FInteger = int32;
			using FUInteger = uint32;
			using FBigInt = int64;
			using FUBigInt = uint64;
			using FByte = char;

			enum class EIsolationLevel
			{
				IL_ReadCommitted,
				IL_RepeatableRead,
				IL_Serializable
			};

			template <> struct TProtocolBinarySelector<FSmallInt> : DbPlus::Detail::IntegralBinaryType { };
			template <> struct TProtocolBinarySelector<FUSmallInt> : DbPlus::Detail::IntegralBinaryType { };
			template <> struct TProtocolBinarySelector<FInteger> : DbPlus::Detail::IntegralBinaryType { };
			template <> struct TProtocolBinarySelector<FUInteger> : DbPlus::Detail::IntegralBinaryType { };
			template <> struct TProtocolBinarySelector<FBigInt> : DbPlus::Detail::IntegralBinaryType { };
			template <> struct TProtocolBinarySelector<FUBigInt> : DbPlus::Detail::IntegralBinaryType { };

            namespace Traits
            {
				template < > struct THasParser<FSmallInt, DbPlus::Detail::EProtocolDataFormat::PDF_Binary> : std::true_type { };
            }
		}
    }
}
