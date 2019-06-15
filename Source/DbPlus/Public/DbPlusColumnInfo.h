#pragma once

#include "CoreMinimal.h"
#include "DbPlusTypes.h"

struct FDbPlusColumnInfo
{
public:
	int32 Index;
	FString Name;
	EDbPlusDataType DataType;

	FDbPlusColumnInfo() = default;
};
