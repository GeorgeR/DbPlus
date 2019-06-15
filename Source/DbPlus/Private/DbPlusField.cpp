#include "DbPlusField.h"

const FString& FDbPlusField::GetName() const
{
}

EDbPlusDataType FDbPlusField::GetDataType() const
{
}

bool FDbPlusField::IsNull() const
{
}

bool FDbPlusField::IsEmpty() const
{
}

template <typename T>
bool FDbPlusField::To(T& OutValue) const
{
	return ToOptional(OutValue, DbPlus::Traits::TIsNullable<T>());
}

template <typename T>
bool FDbPlusField::To(TOptional<T>& OutValue)
{
    if(IsNull())
    {
		OutValue = TOptional<T>();
		return true;
    }
	else
	{
		typename TDecay<T>::Type Temp;

		OutValue = TOptional<T>(Temp);
		return true;
	}
	return false;
}

template <typename T>
typename TDecay<T>::Type FDbPlusField::As() const
{
	typename TDecay<T>::Type Value;
	To(Value);
	return Value;
}

template <typename T>
typename TDecay<T>::Type FDbPlusField::Coalesce(const T& DefaultValue) const
{
	return IsNull() ? DefaultValue : As<T>();
}

template <typename T>
bool FDbPlusField::ToOptional(T& OutValue, const std::true_type&) const
{
	typedef DbPlus::Traits::TNullableTraits<T> NullableTraits;
    if(IsNull())
    {
		NullableTraits::SetNull(OutValue);
		return true;
    }

	return ToImplementation(OutValue, DbPlus::Traits::THasParser<T, Binary>());
}

template <typename T>
bool FDbPlusField::ToOptional(T& OutValue, const std::false_type&) const
{
    if(IsNull())
    {
        // throw that value is null
		return false;
    }

	return ToImplementation(OutValue, DbPlus::Traits::THasParser<T, Binary>());
}
