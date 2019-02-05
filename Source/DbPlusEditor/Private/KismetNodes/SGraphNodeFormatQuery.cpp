#include "KismetNodes/SGraphNodeFormatQuery.h"
#include "Widgets/SBoxPanel.h"
#include "GraphEditorSettings.h"
#include "K2Node_FormatQuery.h"
#include "NodeFactory.h"

void SGraphNodeFormatQuery::Construct(const FArguments& Args, UK2Node_FormatQuery* Node)
{
	this->GraphNode = Node;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SGraphNodeFormatQuery::CreatePinWidgets()
{
	// Create Pin widgets for each of the pins, except for the default pin
	for (auto Iterator = GraphNode->Pins.CreateConstIterator(); Iterator; ++Iterator)
	{
		UEdGraphPin* CurrentPin = *Iterator;
		if ((!CurrentPin->bHidden))
		{
			TSharedPtr<SGraphPin> NewPin = FNodeFactory::CreatePinWidget(CurrentPin);
			check(NewPin.IsValid());

			this->AddPin(NewPin.ToSharedRef());
		}
	}
}

void SGraphNodeFormatQuery::CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox)
{
	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(
		NSLOCTEXT("FormatQueryNode", "FormatQueryNodeAddPinButton", "Add pin"),
		NSLOCTEXT("FormatQueryNode", "FormatQueryNodeAddPinButton_Tooltip", "Adds an argument to the node"),
		false);

	FMargin AddPinPadding = Settings->GetInputPinPadding();
	AddPinPadding.Top += 6.0f;

	InputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
}

EVisibility SGraphNodeFormatQuery::IsAddPinButtonVisible() const
{
	EVisibility VisibilityState = EVisibility::Collapsed;
	if (Cast<UK2Node_FormatQuery>(GraphNode) != nullptr)
	{
		VisibilityState = SGraphNode::IsAddPinButtonVisible();
		if (VisibilityState == EVisibility::Visible)
		{
			UK2Node_FormatQuery* FormatNode = CastChecked<UK2Node_FormatQuery>(GraphNode);
			VisibilityState = FormatNode->CanEditArguments() ? EVisibility::Visible : EVisibility::Collapsed;
		}
	}
	return VisibilityState;
}

FReply SGraphNodeFormatQuery::OnAddPin()
{
	if (UK2Node_FormatQuery* FormatQuery = Cast<UK2Node_FormatQuery>(GraphNode))
		FormatQuery->AddArgumentPin();

	return FReply::Handled();
}

