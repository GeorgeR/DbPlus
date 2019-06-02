#pragma once

#include "CoreMinimal.h"
#include "Future.h"

class FDbConnection;

class FDbTransaction
	: public TSharedFromThis<FDbTransaction>
{
public:
	FDbTransaction(TSharedPtr<FDbConnection> InConnection);
	~FDbTransaction();

	FDbTransaction(const FDbTransaction&) = delete;
	FDbTransaction& operator=(const FDbTransaction&) = delete;
	FDbTransaction(FDbTransaction&&) = delete;
	FDbTransaction& operator=(FDbTransaction&&) = delete;

	TFuture<> Commit();

    template <template<typename> class Promise = TPromise>
	auto Commit() -> decltype(DeclVal<Promise<void>>().GetFuture());

	TFuture<> Rollback();

	template <template<typename> class Promise = TPromise>
	auto Rollback() -> decltype(DeclVal<Promise<void>>().GetFuture());

	TFuture<> Execute(const FString& Query);
	TFuture<> Execute(const FString& Query, const FTypeSequence& ParameterTypes, const TArray<uint8>& ParameterBuffer);
};
