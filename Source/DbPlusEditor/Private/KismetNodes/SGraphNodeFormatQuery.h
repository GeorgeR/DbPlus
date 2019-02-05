#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "KismetNodes/SGraphNodeK2Base.h"

class SVerticalBox;
class UK2Node_FormatQuery;

class SGraphNodeFormatQuery
	: public SGraphNodeK2Base
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeFormatQuery) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args, UK2Node_FormatQuery* Node);

	virtual void CreatePinWidgets() override;

protected:
	virtual void CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox) override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	virtual FReply OnAddPin() override;
};
