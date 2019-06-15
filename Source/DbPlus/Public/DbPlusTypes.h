#pragma once

#include "CoreMinimal.h"

#include "DbPlusTypes.generated.h"

UENUM(BlueprintType)
enum class EDbPlusDataType : uint8
{
    DPDT_Int8                UMETA(DisplayName = "Integer8"),
    DPDT_UInt8               UMETA(DisplayName = "UInteger8"),
    
	DPDT_Int16               UMETA(DisplayName = "Integer16"),
	DPDT_UInt16              UMETA(DisplayName = "UInteger16"),

	DPDT_Int32               UMETA(DisplayName = "Integer32"),
	DPDT_UInt32              UMETA(DisplayName = "UInteger32"),

	DPDT_Int64               UMETA(DisplayName = "Integer64"),
	DPDT_UInt64              UMETA(DisplayName = "UInteger64"),

	DPDT_Float               UMETA(DisplayName = "Float"),
	DPDT_Double              UMETA(DisplayName = "Double"),

	DPDT_String              UMETA(DisplayName = "String"),
	DPDT_Char                UMETA(DisplayName = "Char"),

	DPDT_Boolean             UMETA(DisplayName = "Boolean"),

	DPDT_Unknown             UMETA(DisplayName = "Unknown"),
};
