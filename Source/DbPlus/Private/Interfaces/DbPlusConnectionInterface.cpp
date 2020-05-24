#include "Interfaces/DbPlusConnectionInterface.h"

#include "Launch/Resources/Version.h"

bool IDbPlusConnectionInterface::Connect()
{
	return ReceiveConnect();
}

TFuture<bool> IDbPlusConnectionInterface::ConnectAsync()
{
#if ENGINE_MINOR_VERSION <= 22
    return Async<bool>(EAsyncExecution::TaskGraph, [&]
#else
	return Async(EAsyncExecution::TaskGraph, [&]
#endif
    {
		const auto bWasSuccessful = Connect();
		return bWasSuccessful;
	});
}

bool IDbPlusConnectionInterface::Disconnect()
{
	return ReceiveDisconnect();
}

bool IDbPlusConnectionInterface::IsConnected() const
{
	return ReceiveIsConnected();
}

bool IDbPlusConnectionInterface::IsValid(FString& OutMessage) const
{
	return ReceiveIsValid(OutMessage);
}

bool IDbPlusConnectionInterface::Execute(const FString& Query, const FDbPlusTransaction& Transaction) const
{
	return ReceiveExecute(Query, Transaction);
}

TFuture<bool> IDbPlusConnectionInterface::ExecuteAsync(const FString& Query, const FDbPlusTransaction& Transaction) const
{
#if ENGINE_MINOR_VERSION <= 22
    return Async<bool>(EAsyncExecution::TaskGraph, [&]
#else
	return Async(EAsyncExecution::TaskGraph, [&]
#endif
    {
		const auto bWasSuccessful = Execute(Query, Transaction);
		return bWasSuccessful;
	});
}

bool IDbPlusConnectionInterface::Query(const FString& Query, FDbPlusRecordSet& OutResult, const FDbPlusTransaction& Transaction) const
{
	return ReceiveQuery(Query, OutResult, Transaction);
}

TFuture<FDbPlusResponse> IDbPlusConnectionInterface::QueryAsync(const FString& Query, const FDbPlusTransaction& Transaction) const
{
#if ENGINE_MINOR_VERSION <= 22
    return Async<bool>(EAsyncExecution::TaskGraph, [&]
#else
	return Async(EAsyncExecution::TaskGraph, [&]
#endif
    {
		FDbPlusRecordSet Result;
		const auto bWasSuccessful = this->Query(Query, Result, Transaction);
		return FDbPlusResponse{ bWasSuccessful, MoveTemp(Result) };
	});
}
