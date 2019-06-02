#pragma once

#include "Misc/CoreDefines.h"

template<typename> struct TDbQueryDataTraits;

class IDbQueryTaskInterface
{
public:
	virtual ~IDbQueryTaskInterface() = default;

	virtual bool RunTask() = 0;
	virtual void OnAbandoned() = 0;
};

class FDbQueryTask
	: public IDbQueryTaskInterface
{
public:

private:
	FString Query;
};
