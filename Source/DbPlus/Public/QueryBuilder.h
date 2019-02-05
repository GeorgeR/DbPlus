#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "QueryBuilder.generated.h"

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
