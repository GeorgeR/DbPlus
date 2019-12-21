#include "DbPlusTypes.h"

FDbPlusTransaction FDbPlusTransaction::None = FDbPlusTransaction();

FDbPlusTransaction::FDbPlusTransaction(const FString& InName, const FString& InDescription)
	: Name(InName),
	Description(InDescription)
{
}
