#pragma once

#include "CoreMinimal.h"

#include "DbPlusField.h"

#include "DbPlusRow.generated.h"

USTRUCT(BlueprintType)
struct DBPLUS_API FDbPlusRow
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FString, FDbPlusField> Fields;
};
