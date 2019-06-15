#include "Interfaces/DbPlusConnectionInterface.h"

#include "Async.h"

IDbPlusConnectionInterface::~IDbPlusConnectionInterface()
{
	//Disconnect();
}

TFuture<bool> IDbPlusConnectionInterface::ConnectAsync()
{
    return Async<bool>(EAsyncExecution::TaskGraph, [&]
    {
		const auto bWasSuccessful = Connect();
		return bWasSuccessful;
	});
}

TFuture<bool> IDbPlusConnectionInterface::ExecuteAsync(const FString& Query, const FString& TransactionName) const
{
    return Async<bool>(EAsyncExecution::TaskGraph, [&]
    {
		const auto bWasSuccessful = Execute(Query, TransactionName);
		return bWasSuccessful;
	});
}

TFuture<FDbPlusQueryResult> IDbPlusConnectionInterface::QueryAsync(const FString& Query, const FString& TransactionName) const
{
    return Async<FDbPlusQueryResult>(EAsyncExecution::TaskGraph, [&]
    {
		FDbPlusRecordSet Result;
		const auto bWasSuccessful = this->Query(Query, Result, TransactionName);
		return FDbPlusQueryResult{ bWasSuccessful, MoveTemp(Result) };
	});
}
