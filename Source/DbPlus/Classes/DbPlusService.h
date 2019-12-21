#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"

#include "DbPlusService.generated.h"

class IDbPlusConnectionInterface;

/** This OPTIONAL service is designed to be a singleton that processes requests, connecting to and from the Db when necessary */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DBPLUS_API UDbPlusService
	: public UObject
{
	GENERATED_BODY()

public:
	/** Can connect to Db, etc. */
	UFUNCTION(BlueprintImplementableEvent, Category = "DbPlus|Service")
	bool IsValid() const;
	virtual bool IsValid_Implementation() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "DbPlus|Service")
	TScriptInterface<IDbPlusConnectionInterface> Connect();
	virtual TScriptInterface<IDbPlusConnectionInterface> Connect_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = "DbPlus|Service")
	TScriptInterface<IDbPlusConnectionInterface> Disconnect();
	virtual TScriptInterface<IDbPlusConnectionInterface> Disconnect_Implementation();
};
