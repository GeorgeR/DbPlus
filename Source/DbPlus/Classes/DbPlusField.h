#pragma once

#include "CoreMinimal.h"
#include "Misc/TVariant.h"

#include "DbPlusTypes.h"

#include "DbPlusField.generated.h"

USTRUCT(BlueprintType)
struct DBPLUS_API FDbPlusField
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	FString Name;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	EDbPlusDataType DataType;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsNull;

private:
	TVariant<int8, uint8, int16, uint16, int32, uint32, int64, uint64,
			float, double,
			FString, FChar,
			bool> Value;
	
public:
	FDbPlusField();
	explicit FDbPlusField(EDbPlusDataType InDataType);

    explicit FDbPlusField(const int8& InValue);
    explicit FDbPlusField(const uint8& InValue);
    explicit FDbPlusField(const int16& InValue);
    explicit FDbPlusField(const uint16& InValue);
    explicit FDbPlusField(const int32& InValue);
    explicit FDbPlusField(const uint32& InValue);
    explicit FDbPlusField(const int64& InValue);
    explicit FDbPlusField(const uint64& InValue);

    explicit FDbPlusField(const float& InValue);
    explicit FDbPlusField(const double& InValue);

    explicit FDbPlusField(const FString& InValue);
    explicit FDbPlusField(const FChar& InValue);

    explicit FDbPlusField(const bool& InValue);

	FORCEINLINE const FString& GetName() const { return Name; }

	FORCEINLINE const EDbPlusDataType& GetDataType() const { return DataType; }

	/* if the field is nullable, and is null */
	FORCEINLINE bool IsNull() const { return bIsNull; }

	/* ie. a zero length string */
	FORCEINLINE bool IsEmpty() const { return false; /* @todo */ }

	/* has a known datatype */
	FORCEINLINE bool IsValid() const { return GetDataType() != EDbPlusDataType::DPDT_Unknown; }

public:
	template <typename T>
	typename TDecay<T>::Type As() const;

	template <typename T>
	typename TDecay<T>::Type As(const T& DefaultValue) const;
};

template <>
FORCEINLINE int8 FDbPlusField::As<int8>() const
{
	return As<int8>(0);
}

template <>
FORCEINLINE uint8 FDbPlusField::As<uint8>() const
{
	return As<uint8>(0);
}

template <>
FORCEINLINE int16 FDbPlusField::As<int16>() const
{
	return As<int16>(0);
}

template <>
FORCEINLINE uint16 FDbPlusField::As<uint16>() const
{
	return As<uint16>(0);
}

template <>
FORCEINLINE int32 FDbPlusField::As<int32>() const
{
	return As<int32>(0);
}

template <>
FORCEINLINE uint32 FDbPlusField::As<uint32>() const
{
	return As<uint32>(0);
}

template <>
FORCEINLINE int64 FDbPlusField::As<int64>() const
{
	return As<int64>(0);
}

template <>
FORCEINLINE uint64 FDbPlusField::As<uint64>() const
{
	return As<uint64>(0);
}

template <>
FORCEINLINE float FDbPlusField::As<float>() const
{
	return As<float>(0.0f);
}

template <>
FORCEINLINE double FDbPlusField::As<double>() const
{
	return As<double>(0.0);
}

template <>
FORCEINLINE FString FDbPlusField::As<FString>() const
{
	return As<FString>(TEXT(""));
}

template <>
FORCEINLINE FChar FDbPlusField::As<FChar>() const
{
	return As<FChar>(FChar());
}

template <>
FORCEINLINE bool FDbPlusField::As<bool>() const
{
	return As<bool>(false);
}

template <typename T>
typename TDecay<T>::Type FDbPlusField::As(const T& DefaultValue) const
{
	return IsNull() ? DefaultValue : Value.Get<T>();
}
