#include "DbPlusRequestProcessor.h"

FDbPlusRequestProcessor::FDbPlusRequestProcessor(FDbPlusConnectionProvider& InConnectionProvider)
	: ConnectionProvider(InConnectionProvider)
{
	ensure(ConnectionProvider().GetObject()); // check that the provider is valid
}

void FDbPlusRequestProcessor::ProcessRequests()
{
	auto Connection = ConnectionProvider();
	FString Message;
	if(!Connection->IsValid(Message))
	{
		// #todo
		// UE_LOG(LogDbPlus, )
	}
	
	if(!Connection->IsConnected() && Connection->IsValid(Message))
		if(!Connection->Connect())
			return;

	auto bHasRequests = false;
	TArray<FDbPlusRequest> RequestBatch;

	if(!RequestQueue.IsEmpty())
	{
		FDbPlusRequest Request;
		while(RequestQueue.Dequeue(Request))
		{
			RequestBatch.Add(MoveTemp(Request));
		}
		RequestQueue.Empty();
		bHasRequests = RequestBatch.Num() > 0;
	}

	if(bHasRequests)
	{
		for(const auto& Request : RequestBatch)
		{
			FDbPlusRecordSet RecordSet;
			Connection->Query(Request.Query, RecordSet, FDbPlusTransaction());
		}
	}
}
