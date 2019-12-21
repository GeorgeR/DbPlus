#pragma once

#include "CoreMinimal.h"

#include "DbPlusRow.h"

#include "DbPlusRecordSet.generated.h"

USTRUCT(BlueprintType)
struct DBPLUS_API FDbPlusRecordSet
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<FDbPlusRow> Rows;
	
public:
	typedef TArray<FDbPlusRow>::SizeType SizeType;

	FORCEINLINE SizeType AppendRow(FDbPlusRow&& Row)
	{
		return Rows.Add(MoveTemp(Row));
	}

	FORCEINLINE SizeType Num() const
	{
		return Rows.Num();
	}

	FORCEINLINE FDbPlusRow& operator[](SizeType Index)
	{
		return Rows[Index];
	}

	FORCEINLINE const FDbPlusRow& operator[](SizeType Index) const
	{
		return Rows[Index];
	}
};
