#include "DbPlusQueryBuilder.h"

FDbPlusQueryBuilder::FDbPlusQueryBuilder()
	: bHasWhere(false),
	bIsLocked(false)
{
}

FDbPlusQueryBuilder& FDbPlusQueryBuilder::Select(const FString& TableName)
{
	Command = Command.AppendChar('/s'); // add space
	Command = Command.Append(FString::Printf(TEXT("SELECT * FROM %s"), *TableName));
	
	return *this;
}

FDbPlusQueryBuilder& FDbPlusQueryBuilder::Where(const FString& FieldName, const TCHAR& Operator, const FString& Value)
{
	Command = Command.AppendChar('/s'); // add space

	if(bHasWhere)
		Command = Command.Append(TEXT("WHERE "));
	else
		Command = Command.Append(TEXT(" AND"));

	Command = Command.Append(FString::Printf(TEXT(" %s %c %s"), *FieldName, Operator, *Value));

	bHasWhere = true;
	
	return *this;
}

const FString& FDbPlusQueryBuilder::ToString()
{
	if(bIsLocked)
		return Command;
	
	Command = Command.TrimStartAndEnd(); // remove redundant spaces
	Command = Command.AppendChar(';'); // terminate with semicolon

	bIsLocked = true;
	
	return Command;
}
