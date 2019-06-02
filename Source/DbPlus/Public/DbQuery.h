#pragma once

#include "CoreMinimal.h"
#include "Future.h"

class FDbQuery
{
public:
	FDbQuery(const FDbAlias& Alias, const FString& Expression);

	FDbQuery(const FDbAlias& Alias, const EDbTransactionMode& Mode, const FString& Expression);

	template <typename... Args>
	FDbQuery(const FDbAlias& Alias, const FString& Expression, const Args&... Params);

	template <typename... Args>
	FDbQuery(const FDbAlias& Alias, const EDbTransactionMode& Mode, const FString& Expression, const Args&... Params);

	FDbQuery(TSharedPtr<FDbTransaction> Transaction, const FString& Expression);

	template <typename... Args>
	FDbQuery(TSharedPtr<FDbTransaction> Transaction, const FString& Expression, const Args&... Params);

	template <typename... Args>
	FDbQuery& Bind(const Args&... Params);

	FDbQuery& Bind();

	TFuture<> ExecuteAsync() const;

	TFuture<> operator()() const;

	template <template <typename> class Promise = TPromise>
	auto Execute() const -> decltype(<Promise<FDbResultSet>>().GetFuture());

private:
	using FParameterBuffer = TArray<uint8>;
	struct imp;
	using pimpl = TSharedPtr<impl>;


};
