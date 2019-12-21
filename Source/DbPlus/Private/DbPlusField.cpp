#include "DbPlusField.h"

FDbPlusField::FDbPlusField()
    : DataType(EDbPlusDataType::DPDT_Unknown),
    bIsNull(true) { }

FDbPlusField::FDbPlusField(const EDbPlusDataType InDataType)
    : DataType(InDataType),
    bIsNull(true) { }

FDbPlusField::FDbPlusField(const int8& InValue)
    : DataType(EDbPlusDataType::DPDT_Int8),
    bIsNull(false)
{
	Value.Set<int8>(InValue);
}

FDbPlusField::FDbPlusField(const uint8& InValue)
    : DataType(EDbPlusDataType::DPDT_UInt8),
    bIsNull(false)
{
	Value.Set<uint8>(InValue);
}

FDbPlusField::FDbPlusField(const int16& InValue)
    : DataType(EDbPlusDataType::DPDT_Int16),
    bIsNull(false)
{
	Value.Set<int16>(InValue);
}

FDbPlusField::FDbPlusField(const uint16& InValue)
    : DataType(EDbPlusDataType::DPDT_UInt16),
    bIsNull(false)
{
	Value.Set<uint16>(InValue);
}

FDbPlusField::FDbPlusField(const int32& InValue)
    : DataType(EDbPlusDataType::DPDT_Int32),
    bIsNull(false)
{
	Value.Set<int32>(InValue);
}

FDbPlusField::FDbPlusField(const uint32& InValue)
    : DataType(EDbPlusDataType::DPDT_UInt32),
    bIsNull(false)
{
	Value.Set<uint32>(InValue);
}

FDbPlusField::FDbPlusField(const int64& InValue)
    : DataType(EDbPlusDataType::DPDT_Int64),
    bIsNull(false)
{
	Value.Set<int64>(InValue);
}

FDbPlusField::FDbPlusField(const uint64& InValue)
    : DataType(EDbPlusDataType::DPDT_UInt64),
    bIsNull(false)
{
	Value.Set<uint64>(InValue);
}

FDbPlusField::FDbPlusField(const float& InValue)
    : DataType(EDbPlusDataType::DPDT_Float),
    bIsNull(false)
{
	Value.Set<float>(InValue);
}

FDbPlusField::FDbPlusField(const double& InValue)
    : DataType(EDbPlusDataType::DPDT_Double),
    bIsNull(false)
{
	Value.Set<double>(InValue);
}

FDbPlusField::FDbPlusField(const FString& InValue)
    : DataType(EDbPlusDataType::DPDT_String),
    bIsNull(false)
{
	Value.Set<FString>(InValue);
}

FDbPlusField::FDbPlusField(const FChar& InValue)
    : DataType(EDbPlusDataType::DPDT_Char),
    bIsNull(false)
{
	Value.Set<FChar>(InValue);
}

FDbPlusField::FDbPlusField(const bool& InValue)
    : DataType(EDbPlusDataType::DPDT_Boolean),
    bIsNull(false)
{
	Value.Set<bool>(InValue);
}
