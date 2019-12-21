#pragma once

#include "CoreMinimal.h"

#include "AwesomeGenericRequestProcessor.h"

#include "Interfaces/DbPlusConnectionInterface.h"

typedef TFunction<TScriptInterface<IDbPlusConnectionInterface>()> FDbPlusConnectionProvider;

class DBPLUS_API FDbPlusRequestProcessor
	: public TAwesomeGenericRequestProcessor<FDbPlusRequest, FDbPlusResponse>
{
public:
	explicit FDbPlusRequestProcessor(FDbPlusConnectionProvider& InConnectionProvider);
	
	void ProcessRequests() override;

private:
	FDbPlusConnectionProvider ConnectionProvider;
};
