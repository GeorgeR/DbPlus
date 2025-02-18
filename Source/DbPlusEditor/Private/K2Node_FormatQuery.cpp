#include "K2Node_FormatQuery.h"
#include "UObject/Package.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_CallFunction.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetCompiler.h"
#include "ScopedTransaction.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"

#define LOCTEXT_NAMESPACE "K2Node_FormatQuery"

struct FFormatQueryNodeHelper
{
	static const FName FormatPinName;

	static const FName GetFormatPinName()
	{
		return FormatPinName;
	}
};

const FName FFormatQueryNodeHelper::FormatPinName(TEXT("Format"));

UK2Node_FormatQuery::UK2Node_FormatQuery()
	: CachedFormatPin(nullptr)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Builds a formatted query string using available format argument values.\n  \u2022 Use {} to denote format arguments.");
}

void UK2Node_FormatQuery::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CachedFormatPin = CreatePin(EEdGraphPinDirection::EGPD_Input, UEdGraphSchema_K2::PC_String, FFormatQueryNodeHelper::GetFormatPinName());
	CreatePin(EEdGraphPinDirection::EGPD_Output, UEdGraphSchema_K2::PC_String, TEXT("Result"));

	for (const auto& PinName : PinNames)
		CreatePin(EEdGraphPinDirection::EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, PinName);
}

void UK2Node_FormatQuery::SynchronizeArgumentPinType(UEdGraphPin* Pin)
{
	const UEdGraphPin* FormatPin = GetFormatPin();
	if (Pin != FormatPin && Pin->Direction == EEdGraphPinDirection::EGPD_Input)
	{
		const UEdGraphSchema_K2* K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
		bool bPinTypeChanged = false;
		if (Pin->LinkedTo.Num() == 0)
		{
			static const FEdGraphPinType WildcardPinType = FEdGraphPinType(UEdGraphSchema_K2::PC_Wildcard, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType());
			if (Pin->PinType != WildcardPinType)
			{
				Pin->PinType = WildcardPinType;
				bPinTypeChanged = true;
			}
		}
		else
		{
			UEdGraphPin* ArgumentSourcePin = Pin->LinkedTo[0];
			if (Pin->PinType != ArgumentSourcePin->PinType)
			{
				Pin->PinType = ArgumentSourcePin->PinType;
				bPinTypeChanged = true;
			}
		}

		if (bPinTypeChanged)
		{
			GetGraph()->NotifyGraphChanged();

			UBlueprint* Blueprint = GetBlueprint();
			if (!Blueprint->bBeingCompiled)
			{
				FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
				Blueprint->BroadcastChanged();
			}
		}
	}
}

FText UK2Node_FormatQuery::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("FormatQuery_Title", "Format Query");
}

FText UK2Node_FormatQuery::GetPinDisplayName(const UEdGraphPin* Pin) const
{
	return FText::FromName(Pin->PinName);
}

FName UK2Node_FormatQuery::GetUniquePinName()
{
	FName NewPinName;
	int32 i = 0;
	while (true)
	{
		NewPinName = *FString::FromInt(i++);
		if (!FindPin(NewPinName))
			break;
	}
	return NewPinName;
}

void UK2Node_FormatQuery::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None);
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UK2Node_FormatQuery, PinNames))
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UK2Node_FormatQuery::PinConnectionListChanged(UEdGraphPin* Pin)
{
	UEdGraphPin* FormatPin = GetFormatPin();
	Modify();

	if (Pin == FormatPin && !FormatPin->DefaultTextValue.IsEmpty())
	{
		PinNames.Empty();
		GetSchema()->TrySetDefaultText(*FormatPin, FText::GetEmpty());

		for (auto Iterator = Pins.CreateConstIterator(); Iterator; ++Iterator)
		{
			UEdGraphPin* CheckPin = *Iterator;
			if (CheckPin != FormatPin && CheckPin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				CheckPin->Modify();
				CheckPin->MarkPendingKill();
				Pins.Remove(CheckPin);
				--Iterator;
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}

	SynchronizeArgumentPinType(Pin);
}

void UK2Node_FormatQuery::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	const UEdGraphPin* FormatPin = GetFormatPin();
	if (Pin == FormatPin && FormatPin->LinkedTo.Num() == 0)
	{
		TArray<FString> ArgumentParameters;
		FText FormatString = FText::FromString(FormatPin->DefaultValue);
		FText::GetFormatPatternParameters(FormatString, ArgumentParameters);

		PinNames.Reset();

		for (const auto& Parameter : ArgumentParameters)
		{
			const FName ParameterName(*Parameter);
			if (!FindArgumentPin(ParameterName))
				CreatePin(EEdGraphPinDirection::EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, ParameterName);

			PinNames.Add(ParameterName);
		}

		for (auto Iterator = Pins.CreateIterator(); Iterator; ++Iterator)
		{
			UEdGraphPin* CheckPin = *Iterator;
			if (CheckPin != FormatPin && CheckPin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				const bool bIsValidArgumentPin = ArgumentParameters.ContainsByPredicate([&CheckPin](const FString& PinName) {
					return PinName.Equals(CheckPin->PinName.ToString(), ESearchCase::CaseSensitive);
				});

				if (!bIsValidArgumentPin)
				{
					CheckPin->MarkPendingKill();
					Iterator.RemoveCurrent();
				}
			}
		}

		GetGraph()->NotifyGraphChanged();
	}
}

void UK2Node_FormatQuery::PinTypeChanged(UEdGraphPin* Pin)
{
	SynchronizeArgumentPinType(Pin);
	Super::PinTypeChanged(Pin);
}

FText UK2Node_FormatQuery::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* FindOutputStructPinChecked(UEdGraphNode* Node)
{
	check(Node != nullptr);
	
	UEdGraphPin* OutputPin = nullptr;
	for (auto i = 0; i < Node->Pins.Num(); i++)
	{
		UEdGraphPin* Pin = Node->Pins[i];
		if (Pin && (Pin->Direction = EEdGraphPinDirection::EGPD_Output))
		{
			OutputPin = Pin;
			break;
		}
	}

	check(OutputPin != nullptr);
	
	return OutputPin;
}

void UK2Node_FormatQuery::PostReconstructNode()
{
	Super::PostReconstructNode();

	// TODO: What's this do? Do I need it?
	if (!IsTemplate())
	{
		UEdGraph* OuterGraph = GetGraph();
		if (OuterGraph && OuterGraph->Schema)
		{
			int32 NumPinsFixedUp = 0;

			const UEdGraphPin* FormatPin = GetFormatPin();
			for (UEdGraphPin* CurrentPin : Pins)
			{
				if (CurrentPin != FormatPin
					&& CurrentPin->Direction == EEdGraphPinDirection::EGPD_Input
					&& CurrentPin->LinkedTo.Num() == 0
					&& !CurrentPin->DefaultValue.IsEmpty())
				{
					const FVector2D SpawnLocation = FVector2D(NodePosX - 300, NodePosY + (60 * NumPinsFixedUp + 1));
					
				}
			}
		}
	}
}

void UK2Node_FormatQuery::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	/**
		At the end of this, the UK2Node_FormatQuery will not be a part of the Blueprint, it merely handles connecting
		the other nodes into the Blueprint.
	*/

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	// Create a "Make Array" node to compile the list of arguments into an array for the Format function being called
	UK2Node_MakeArray* MakeArrayNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
	MakeArrayNode->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(MakeArrayNode, this);

	UEdGraphPin* ArrayOut = MakeArrayNode->GetOutputPin();

	// This is the node that does all the Format work.
	UK2Node_CallFunction* CallFormatFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFormatFunction->SetFromFunction(UKismetTextLibrary::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UKismetTextLibrary, Format)));
	CallFormatFunction->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallFormatFunction, this);

	// This will set the "Make Array" node's type, only works if one pin is connected.
	ArrayOut->MakeLinkTo(CallFormatFunction->FindPinChecked(TEXT("InArgs")));

	// This will set the "Make Array" node's type, only works if one pin is connected.
	MakeArrayNode->PinConnectionListChanged(ArrayOut);

	// For each argument, we will need to add in a "Make Struct" node.
	for (auto i = 0; i < PinNames.Num(); i++)
	{
		UEdGraphPin* ArgumentPin = FindArgumentPin(PinNames[i]);

		static UScriptStruct* FormatArgumentDataStruct = FindObjectChecked<UScriptStruct>(FindObjectChecked<UPackage>(nullptr, TEXT("/Script/Engine")), TEXT("FormatArgumentData"));

		// Spawn a "Make Struct" node to create the struct needed for formatting the text.
		UK2Node_MakeStruct* MakeFormatArgumentDataStruct = CompilerContext.SpawnIntermediateNode<UK2Node_MakeStruct>(this, SourceGraph);
		MakeFormatArgumentDataStruct->StructType = FormatArgumentDataStruct;
		MakeFormatArgumentDataStruct->AllocateDefaultPins();
		MakeFormatArgumentDataStruct->bMadeAfterOverridePinRemoval = true;
		CompilerContext.MessageLog.NotifyIntermediateObjectCreation(MakeFormatArgumentDataStruct, this);

		// Set the struct's "ArgumentName" pin literal to be the argument pin's name.
		MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentName)), ArgumentPin->PinName.ToString());

		UEdGraphPin* ArgumentTypePin = MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValueType));

		// Move the connection of the argument pin to the correct argument value pin, and also set the correct argument type based on the pin that was hooked up.
		if (ArgumentPin->LinkedTo.Num() > 0)
		{
			const FName& ArgumentPinCategory = ArgumentPin->PinType.PinCategory;

			if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Boolean)
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Boolean"));
				//CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValueBool)))
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Byte && !ArgumentPin->PinType.PinSubCategoryObject.IsValid())
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Int"));

				// Need a manual cast from byte -> int
				UK2Node_CallFunction* CallByteToIntFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
				CallByteToIntFunction->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UKismetMathLibrary, Conv_ByteToInt)));
				CallByteToIntFunction->AllocateDefaultPins();
				CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallByteToIntFunction, this);

				// Move the byte output pin to the input pin of the conversion node
				CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *CallByteToIntFunction->FindPinChecked(TEXT("InByte")));

				// Connect the int output pin to the argument value
				CallByteToIntFunction->FindPinChecked(TEXT("ReturnValue"))->MakeLinkTo(MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValueInt)));
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Byte || ArgumentPinCategory == UEdGraphSchema_K2::PC_Enum)
			{
				static UEnum* TextGenderEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("ETextGender"), /*ExactClass*/true);
				if (ArgumentPin->PinType.PinSubCategoryObject == TextGenderEnum)
				{
					MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Gender"));
					CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValueGender)));
				}
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Int)
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Int"));
				CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValueInt)));
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Float)
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Float"));
				CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValueFloat)));
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Name)
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Name"));
				CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValue)));
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_String)
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("String"));
				CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValue)));
			}
			else if (ArgumentPinCategory == UEdGraphSchema_K2::PC_Text)
			{
				MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Text"));
				CompilerContext.MovePinLinksToIntermediate(*ArgumentPin, *MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValue)));
			}
			else
			{
				// Unexpected pin type!
				CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("Error_UnexpectedPinType", "Pin '{0}' has an unexpected type: {1}"), FText::FromName(PinNames[i]), FText::FromName(ArgumentPinCategory)).ToString());
			}
		}
		else
		{
			// No connected pin - just default to an empty text
			MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultValue(*ArgumentTypePin, TEXT("Text"));
			MakeFormatArgumentDataStruct->GetSchema()->TrySetDefaultText(*MakeFormatArgumentDataStruct->FindPinChecked(GET_MEMBER_NAME_STRING_CHECKED(FFormatArgumentData, ArgumentValue)), FText::GetEmpty());
		}

		// The "Make Array" node already has one pin available, so don't create one for ArgIdx == 0
		if (i > 0)
			MakeArrayNode->AddInputPin();

		// Find the input pin on the "Make Array" node by index.
		const FString PinName = FString::Printf(TEXT("[%d]"), i);
		UEdGraphPin* InputPin = MakeArrayNode->FindPinChecked(PinName);

		// Find the output for the pin's "Make Struct" node and link it to the corresponding pin on the "Make Array" node.
		FindOutputStructPinChecked(MakeFormatArgumentDataStruct)->MakeLinkTo(InputPin);
	}

	// Move connection of FormatText's "Result" pin to the call function's return value pin.
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Result")), *CallFormatFunction->GetReturnValuePin());
	// Move connection of FormatText's "Format" pin to the call function's "InPattern" pin
	CompilerContext.MovePinLinksToIntermediate(*GetFormatPin(), *CallFormatFunction->FindPinChecked(TEXT("InPattern")));

	BreakAllNodeLinks();
}

UEdGraphPin* UK2Node_FormatQuery::FindArgumentPin(const FName PinName) const
{
	const UEdGraphPin* FormatPin = GetFormatPin();
	for (UEdGraphPin* Pin : Pins)
		if (Pin != FormatPin && Pin->Direction != EGPD_Output && Pin->PinName.ToString().Equals(PinName.ToString(), ESearchCase::CaseSensitive))
			return Pin;

	return nullptr;
}

UK2Node::ERedirectType UK2Node_FormatQuery::DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	ERedirectType RedirectType = ERedirectType_None;

	// if the pin names do match
	if (NewPin->PinName.ToString().Equals(OldPin->PinName.ToString(), ESearchCase::CaseSensitive))
	{
		// Make sure we're not dealing with a menu node
		UEdGraph* OuterGraph = GetGraph();
		if (OuterGraph && OuterGraph->Schema)
		{
			const UEdGraphSchema_K2* K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
			if (!K2Schema || K2Schema->IsSelfPin(*NewPin) || K2Schema->ArePinTypesCompatible(OldPin->PinType, NewPin->PinType))
				RedirectType = ERedirectType_Name;
			else
				RedirectType = ERedirectType_None;
		}
	}
	else
	{
		// try looking for a redirect if it's a K2 node
		if (UK2Node* Node = Cast<UK2Node>(NewPin->GetOwningNode()))
		{
			// if you don't have matching pin, now check if there is any redirect param set
			TArray<FString> OldPinNames;
			GetRedirectPinNames(*OldPin, OldPinNames);

			FName NewPinName;
			RedirectType = ShouldRedirectParam(OldPinNames, /*out*/ NewPinName, Node);

			// make sure they match
			if ((RedirectType != ERedirectType_None) && (!NewPin->PinName.ToString().Equals(NewPinName.ToString(), ESearchCase::CaseSensitive)))
				RedirectType = ERedirectType_None;
		}
	}

	return RedirectType;
}

bool UK2Node_FormatQuery::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	// Argument input pins may only be connected to Byte, Integer, Float, Text, and ETextGender pins...
	const UEdGraphPin* FormatPin = GetFormatPin();
	if (MyPin != FormatPin && MyPin->Direction == EGPD_Input)
	{
		const UEdGraphSchema_K2* K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
		const FName& OtherPinCategory = OtherPin->PinType.PinCategory;

		bool bIsValidType = false;
		if (OtherPinCategory == UEdGraphSchema_K2::PC_Boolean
			|| OtherPinCategory == UEdGraphSchema_K2::PC_Int
			|| OtherPinCategory == UEdGraphSchema_K2::PC_Float
			|| OtherPinCategory == UEdGraphSchema_K2::PC_Name
			|| OtherPinCategory == UEdGraphSchema_K2::PC_String
			|| OtherPinCategory == UEdGraphSchema_K2::PC_Text
			|| (OtherPinCategory == UEdGraphSchema_K2::PC_Byte && !OtherPin->PinType.PinSubCategoryObject.IsValid()))
			bIsValidType = true;
		else if (OtherPinCategory == UEdGraphSchema_K2::PC_Byte || OtherPinCategory == UEdGraphSchema_K2::PC_Enum)
		{
			static UEnum* TextGenderEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("ETextGender"), /*ExactClass*/true);
			if (OtherPin->PinType.PinSubCategoryObject == TextGenderEnum)
				bIsValidType = true;
		}

		if (!bIsValidType)
		{
			OutReason = LOCTEXT("Error_InvalidArgumentType", "Format arguments may only be Byte, Integer, Float, Text, or ETextGender.").ToString();
			return true;
		}
	}

	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

FName UK2Node_FormatQuery::GetArgumentName(int32 Index) const
{
	if (Index < PinNames.Num())
		return PinNames[Index];
	
	return NAME_None;
}

void UK2Node_FormatQuery::AddArgumentPin()
{
	const FScopedTransaction Transaction(NSLOCTEXT("Kismet", "AddArgumentPin", "Add Argument Pin"));
	Modify();

	const FName PinName(GetUniquePinName());
	CreatePin(EEdGraphPinDirection::EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, PinName);
	PinNames.Add(PinName);

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	GetGraph()->NotifyGraphChanged();
}

void UK2Node_FormatQuery::RemoveArgument(int32 Index)
{
	const FScopedTransaction Transaction(NSLOCTEXT("Kismet", "RemoveArgumentPin", "Remove Argument Pin"));
	Modify();

	if (UEdGraphPin* ArgumentPin = FindArgumentPin(PinNames[Index]))
	{
		Pins.Remove(ArgumentPin);
		ArgumentPin->MarkPendingKill();
	}

	PinNames.RemoveAt(Index);

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	GetGraph()->NotifyGraphChanged();
}

void UK2Node_FormatQuery::SetArgumentName(int32 Index, FName Name)
{
	PinNames[Index] = Name;
	ReconstructNode();
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

void UK2Node_FormatQuery::SwapArguments(int32 IndexA, int32 IndexB)
{
	check(IndexA < PinNames.Num());
	check(IndexB < PinNames.Num());
	PinNames.Swap(IndexA, IndexB);

	ReconstructNode();
	GetGraph()->NotifyGraphChanged();

	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

UEdGraphPin* UK2Node_FormatQuery::GetFormatPin() const
{
	if (!CachedFormatPin)
		const_cast<UK2Node_FormatQuery*>(this)->CachedFormatPin = FindPinChecked(FFormatQueryNodeHelper::GetFormatPinName());

	return CachedFormatPin;
}

void UK2Node_FormatQuery::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_FormatQuery::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::String);
}

#undef LOCTEXT_NAMESPACE
