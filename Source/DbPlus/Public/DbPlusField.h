#pragma once

#include "CoreMinimal.h"

#include "DbPlusTypes.h"

struct FDbPlusField
{
public:
	FDbPlusField() = default;

	const FString& GetName() const;
	EDbPlusDataType GetDataType() const;
	bool IsNull() const;
	bool IsEmpty() const;

	template <typename T>
	bool To(T& OutValue) const;

	template <typename T>
	bool To(TOptional<T>& OutValue);
	
	template <typename T>
    typename TDecay<T>::Type As() const;

	template <typename T>
	typename TDecay<T>::Type Coalesce(const T& DefaultValue) const;

private:
	EDbPlusDataType DataType;
	bool bIsNull;

    template <typename T>
	bool ToOptional(T& OutValue, const std::true_type&) const;

	template <typename T>
	bool ToOptional(T& OutValue, const std::false_type&) const;

    template <typename T>
    bool ToImplementation(T& OutValue, const std::true_type&) const
    {

        
    }

    template <typename T>
    bool ToImplementation(T& OutValue, const std::false_type&) const
    {
        
    }
};
