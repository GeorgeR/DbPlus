#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Async.h"
#include "Future.h"

#include "DbPlusTypes.h"
#include "DbPlusRecordSet.h"

#include "DbPlusConnectionInterface.generated.h"

USTRUCT(BlueprintType)
struct DBPLUS_API FDbPlusResponse
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Response")
	bool bWasSuccessful;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Response")
	FDbPlusRecordSet RecordSet;

	FDbPlusResponse() = default;
};

USTRUCT(BlueprintType)
struct DBPLUS_API FDbPlusRequest
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Request")
    FString Query;

    FDbPlusRequest() = default;
	explicit FDbPlusRequest(const FString& InQuery) : Query(InQuery) { }

public:
    TSharedPtr<TPromise<FDbPlusResponse>>& GetPromise() { return Promise; }
private:
    TSharedPtr<TPromise<FDbPlusResponse>> Promise;
};

UINTERFACE(MinimalAPI)
class UDbPlusConnectionInterface : public UInterface
{
	GENERATED_BODY()
};

class DBPLUS_API IDbPlusConnectionInterface
{
	GENERATED_BODY()

public:
	//virtual ~IDbPlusConnectionInterface();

	FORCEINLINE virtual bool Connect();
	virtual TFuture<bool> ConnectAsync();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DbPlus|Connection", meta = (DisplayName = "Connect"))
	bool ReceiveConnect();

	FORCEINLINE virtual bool Disconnect();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DbPlus|Connection", meta = (DisplayName = "Disconnect"))
	bool ReceiveDisconnect();

	FORCEINLINE virtual bool IsConnected() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DbPlus|Connection", meta = (DisplayName = "Is Connected"))
	bool ReceiveIsConnected() const;

	FORCEINLINE virtual bool IsValid(FString& OutMessage) const;

	/** ie. the connection string might be invalid */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DbPlus|Connection", meta = (DisplayName = "Is Valid"))
	bool ReceiveIsValid(FString& OutMessage) const;

	FORCEINLINE virtual bool Execute(const FString& Query, const FDbPlusTransaction& Transaction = FDbPlusTransaction()) const;;
	virtual TFuture<bool> ExecuteAsync(const FString& Query, const FDbPlusTransaction& Transaction = FDbPlusTransaction()) const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DbPlus|Connection", meta = (AutoCreateRefTerm = "Transaction", CPP_Default_Transaction, DisplayName = "Execute"))
	bool ReceiveExecute(const FString& Query, const FDbPlusTransaction& Transaction = FDbPlusTransaction()) const;

	FORCEINLINE virtual bool Query(const FString& Query, FDbPlusRecordSet& OutResult, const FDbPlusTransaction& Transaction = FDbPlusTransaction()) const;
	virtual TFuture<FDbPlusResponse> QueryAsync(const FString& Query, const FDbPlusTransaction& Transaction = FDbPlusTransaction()) const;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DbPlus|Connection", meta = (AutoCreateRefTerm = "Transaction", CPP_Default_Transaction, DisplayName = "Query"))
	bool ReceiveQuery(const FString& Query, FDbPlusRecordSet& OutResult, const FDbPlusTransaction& Transaction = FDbPlusTransaction()) const;
};
