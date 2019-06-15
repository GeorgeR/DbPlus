#pragma once

#include "CoreMinimal.h"
#include "Future.h"

#include "DbPlusRecordSet.h"

struct FDbPlusQueryResult
{
public:
	bool bWasSuccessfull;
	FDbPlusRecordSet RecordSet;
};

class IDbPlusConnectionInterface
{
public:
	virtual ~IDbPlusConnectionInterface();

	virtual bool Connect() = 0;
	virtual TFuture<bool> ConnectAsync();

	virtual void Disconnect() = 0;

	virtual bool IsOpen() const = 0;

	virtual bool Execute(const FString& Query, const FString& TransactionName = TEXT("")) const = 0;
	virtual TFuture<bool> ExecuteAsync(const FString& Query, const FString& TransactionName = TEXT("")) const;

	virtual bool Query(const FString& Query, FDbPlusRecordSet& OutResult, const FString& TransactionName = TEXT("")) const = 0;
	virtual TFuture<FDbPlusQueryResult> QueryAsync(const FString& Query, const FString& TransactionName = TEXT("")) const;
};
