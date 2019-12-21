#pragma once

#include "CoreMinimal.h"

#include "DbPlusQueryBuilder.generated.h"

class DBPLUS_API FDbPlusQueryBuilder
{
private:
	bool bHasWhere;
	bool bIsLocked;
	FString Command;

public:
	FDbPlusQueryBuilder();
	
	FDbPlusQueryBuilder& Select(const FString& TableName);
	FDbPlusQueryBuilder& Where(const FString& FieldName, const TCHAR& Operator, const FString& Value);
	
	const FString& ToString();
};

UCLASS(BlueprintType)
class DBPLUS_API UQueryBuilder final
	: public UObject
{
	GENERATED_BODY()

public:

};
