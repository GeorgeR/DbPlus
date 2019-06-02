#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DbQueryBuilder.generated.h"

class UTableMap;

UCLASS()
class DBPLUS_API UQueryBuilder
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DbPlus")
	static UTableMap* GetTableMap(UField* Type);
};
