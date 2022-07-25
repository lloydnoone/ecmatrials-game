// Copyright Prestige Games World Wide.


#include "CodeEditor.h"
#include "Components/MultiLineEditableText.h"
#include "Components/EditableText.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HttpService.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CodeEditorComponent.h"
#include "EcmaEnemyCharacter.h"

#define LOCTEXT_NAMESPACE "EcmaTrials"

bool UCodeEditor::Initialize()
{
	Super::Initialize();

	if (!TextInput)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextInput is nullptr"));
		return false;
	}

	if (!ResponseOutput)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResponseOutput is nullptr"));
		return false;
	}

	if (!SyntaxHighlight)
	{
		UE_LOG(LogTemp, Warning, TEXT("SyntaxHighlight is nullptr"));
		return false;
	}

	//add function to onTextCommitted
	TextInput->OnTextCommitted.AddDynamic(this, &UCodeEditor::DelegateCommitInputText);
	TextInput->OnTextChanged.AddDynamic(this, &UCodeEditor::DelegateOnTextChanged);

	GetHttpService();
	
	return true;
}

void UCodeEditor::GetHttpService()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHttpService::StaticClass(), FoundActors);

	if (FoundActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Http service not found by code editor"));
		return;
	}

	AActor* firstActor = FoundActors[0];

	HttpService = Cast<AHttpService>(firstActor);
	if (!HttpService)
	{
		UE_LOG(LogTemp, Warning, TEXT("Http service cast failed"));
		return;
	}
}

void UCodeEditor::DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType)
{
	if (!HttpService)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Http service for post"));
		return;
	}

	FRequest_PostCode PostCode;
	PostCode.snippet = InText.ToString();
	HttpService->PostCode(PostCode, this, RequestUrl);
}

void UCodeEditor::DelegateOnTextChanged(const FText& InText)
{
	HighlightSyntax(InText.ToString());
}

//catch input before its sent to textinput
FReply UCodeEditor::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//return focus to game to automatically fire off commitinputtext
	if (InKeyEvent.GetKey().ToString() == "LeftControl")
	{
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		return FReply::Handled();
	};

	return  FReply::Unhandled();
}

//FReply UCodeEditor::NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent)
//{
//	UE_LOG(LogTemp, Warning, TEXT("from text input: %s"), InCharEvent.GetCharacter());
//
//	return  FReply::Unhandled();
//}
//
//FString String = TextInput->GetText().ToString() + InCharEvent.GetCharacter();
//FText Text = FText::FromString(String);
//SyntaxHighlight->SetText(Text);

void UCodeEditor::SetOwningActor(AActor* Actor)
{
	OwningActor = Actor;
}

AActor* UCodeEditor::GetOwningActor()
{
	return OwningActor;
}

UCodeEditorComponent* UCodeEditor::GetActorsEditorComponent()
{
	AEcmaEnemyCharacter* Owner = Cast<AEcmaEnemyCharacter>(OwningActor);
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget couldnt cast to actor to Enemy"));
		return NULL;
	}
	UActorComponent* Comp = Owner->FindComponentByClass(UCodeEditorComponent::StaticClass());
	if (!Comp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget couldnt find editor component"));
		return NULL;
	}
	UCodeEditorComponent* EditorComp = Cast<UCodeEditorComponent>(Comp);
	if (!EditorComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget couldnt cast to editor comp"));
		return NULL;
	}
	else
	{
		return EditorComp;
	}
}

int32 UCodeEditor::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!OwningActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Code Editor has no owning interactable"));
		return LayerId;
	}

	// draw line from widget to actor
	FVector2D InteractableLocation;

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetWorld()->GetFirstPlayerController(),
		OwningActor->GetActorLocation(),
		InteractableLocation,
		false
	);

	// get widget geometry
	FVector2D ScreenEnd = TextInput->GetCachedGeometry().AbsoluteToLocal(InteractableLocation);

	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	// get top right corner of widget
	FVector2D ScreenStart = TextInput->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(1.f, 0.f));
	UWidgetBlueprintLibrary::DrawLine(Context, ScreenStart, InteractableLocation, FLinearColor::White, true, 2.0f);
	
	return LayerId;
}

void UCodeEditor::HighlightSyntax(FString RawInput) const
{
	RawInput = TextInput->GetText().ToString();

	//tag symbols
	//must do / < > first to avoid problems breaking tags
	TArray<FString> Symbols = { "<",">","/","+","-","%","*",";",":","|","=","!","?" };
	for (FString Symbol : Symbols)
	{
		FString SymbolReplace = "<symbol>" + Symbol + "</>";
		RawInput.ReplaceInline(*Symbol, *SymbolReplace, ESearchCase::CaseSensitive);
	}

	// tag keywords
	TArray<FString> Keywords = { "const","let" };
	for (FString Keyword : Keywords)
	{
		FString KeywordReplace = "<keyword>" + Keyword + "</>";
		RawInput.ReplaceInline(*Keyword, *KeywordReplace, ESearchCase::CaseSensitive);
	}

	// tag bools
	TArray<FString> Bools = { "true","false" };
	for (FString Bool : Bools)
	{
		FString BoolReplace = "<bool>" + Bool + "</>";
		RawInput.ReplaceInline(*Bool, *BoolReplace, ESearchCase::CaseSensitive);
	}

	//array of tag types, used to clear strings inner text of tags
	TArray<FString> Tags = { "<keyword>","<symbol>","</>" };

	// tag strings
	const FRegexPattern WholeStringPattern(TEXT("\"([^\"]+)?\"?"));
	FRegexMatcher WholeStringMatcher(WholeStringPattern, RawInput);

	//keep track of current index
	int32 CurrentIdx = 0;

	while (WholeStringMatcher.FindNext())
	{
		FString Capture = WholeStringMatcher.GetCaptureGroup(0);

		// get index of capture found after our current index
		int32 CaptureIdx = RawInput.Find(Capture, ESearchCase::CaseSensitive, ESearchDir::FromStart, CurrentIdx);

		//construct replace string
		FString ReplaceString = "<string>" + Capture + "</>";

		RawInput.RemoveAt(CaptureIdx, Capture.Len());
		RawInput.InsertAt(CaptureIdx, ReplaceString);

		// update current index to only search remaining chars
		CurrentIdx = CaptureIdx + ReplaceString.Len();
	}

	FString FormattedString = RawInput;

	FText Text = FText::FromString(FormattedString);

	//update syntax highlighter to latest
	SyntaxHighlight->SetText(Text);
}

//FString UCodeEditor::RegexReplace(FString Regex, FString String, FString ReplaceString) const
//{
//
//	// group pattern
//	const FRegexPattern GroupPattern(Regex);
//	FRegexMatcher GroupMatcher(GroupPattern, String);
//
//	
//	TMap<FString, int32> CaptureMap;
//	int32 Counter = 0;
//	int32 CurrentIdx = 0;
//
//	//store capture groups in map with current idx
//	while (GroupMatcher.FindNext())
//	{
//		bool bGroupsCollected = false;
//		while (!bGroupsCollected)
//		{
//			FString Capture = GroupMatcher.GetCaptureGroup(Counter);
//			if (Capture.Len() == 0)
//			{
//				// if this capture is empty, stop collecting
//				bGroupsCollected = true;
//				break;
//			}
//
//			//get index of capture in string
//			int32 CaptureIdx = String.Find(Capture, ESearchCase::CaseSensitive, ESearchDir::FromStart, CurrentIdx);
//
//			// update current index to only search remaining chars
//			CurrentIdx = CurrentIdx + Capture.Len();
//
//			// add the variable and its starting index in the string to the map
//			CaptureMap.Add(Capture, CaptureIdx);
//			UE_LOG(LogTemp, Warning, TEXT("variable 0: %s"), *GroupMatcher.GetCaptureGroup(Counter));
//
//			Counter++;
//		}
//	}
//
//	TMultiMap<int32, AActor*> exampleIntegerToActorMap;
//	for (const TPair<FString, int32>& pair : CaptureMap)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("key: %s"), *pair.Key);
//		UE_LOG(LogTemp, Warning, TEXT("Value: %i"), pair.Value);
//	}
//
//	// store variables in map with current idx
//	TMultiMap<int32, FString> VariableMap;
//	//reset counter and idx
//	Counter = 0;
//	CurrentIdx = 0;
//
//
//
//
//	return "test";
//}

void UCodeEditor::ReceiveResponse(FResponse_PostCode Response)
{
	// display response output to player
	if (Response.message == "Test passed")
	{
		DisplayOutput(Response.message, false);
	}
	else if (Response.message == "Test failed")
	{
		FString FomattedString = FString(Response.error.name + "\n" + Response.error.message);
		DisplayOutput(FomattedString, true);
	}
	else
	{
		//FString LineNumber = Response.error.stack.Mid(6,2).TrimEnd();
		FString FomattedString = FString("Line Number: " + Response.error.LineNumber + "\n" + Response.error.name + "\n" + Response.error.message);
		DisplayOutput(FomattedString, true);
	}

	//get this actors editor component and use it to handle results
	UCodeEditorComponent* EditorComp = OwningActor->FindComponentByClass< UCodeEditorComponent >(); // ::StaticClass());
	if (EditorComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("cant find actors editor component"));
		return;
	}
	EditorComp->SendResultToSubjectActor(Response.error.name.IsEmpty());
}

void UCodeEditor::DisplayOutput(FString String, bool IsError)
{
	FSlateColor Color = FSlateColor(IsError ? FLinearColor::Red : FLinearColor::Green);
	ResponseOutput->SetText(FText::FromString(String));
	ResponseOutput->SetColorAndOpacity(Color);
	PlayAnimation(SlideIn);
}

void UCodeEditor::SetRequestUrl(FString Url)
{
	RequestUrl = Url;
}

void UCodeEditor::AddEditorToScreen()
{
	AddToViewport();
	// this keyboard focus line doesntseem to work in packaged build, focus is set again later
	TextInput->SetKeyboardFocus();

	// get previous code fron last commited input that was cleared and restore it
	TextInput->SetText(PreviousCode);
	HighlightSyntax(PreviousCode.ToString());

	PlayAnimation(PopOut);
}

void UCodeEditor::RemoveEditorFromScreen()
{
	PreviousCode = TextInput->GetText();
	RemoveFromViewport();
}

//code below was an attempt to fire off onsubmittext, might be usefull

//FEventReply UAccessPanelInterface::OnPreviewKeyDown(FGeometry MyGeometry, FKeyEvent InKeyEvent)
//{
//	UE_LOG(LogTemp, Warning, TEXT("InKeyEvent is: %s"), *InKeyEvent.GetKey().ToString());
//	FEventReply Reply;
//	Reply.NativeReply = FReply::Handled();
//	return Reply;
//}

//UAccessPanelInterface::UAccessPanelInterface(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//}
//
//void UAccessPanelInterface::AddDelegates()
//{
//	EditableText_IN->OnTextCommitted.AddDynamic(this, &UAccessPanelInterface::DelegateCommitInputText);
//
//	// Crude way to see if we past this way
//	testNumber = 20000;
//}
//
//void UAccessPanelInterface::DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType)
//{
//	if (InCommitType == ETextCommit::OnEnter)
//	{
//		// Crude way to see if we also past this way
//		testNumber >>= 1;
//
//		ft_TheStorySofar = FText::FromString(FString::Printf(TEXT("%s%s %s"), *(ft_TheStorySofar.ToString()), *(FString::FromInt(testNumber)), *(InText.ToString())));
//			FText tempText = FText::FromString(FString::Printf(TEXT("Via DELEGATE%s"), *(ft_TheStorySofar.ToString())));
//				MultiLineEditableText_OUT->SetText(tempText);
//	}
//}
//
//void UAccessPanelInterface::BlueprintCommitInputText(const FText& InText)
//{
//	++testNumber; // <-- TEMP for test
//
//	ft_TheStorySofar = FText::FromString(FString::Printf(TEXT("%s%s %s"), *(ft_TheStorySofar.ToString()), *(FString::FromInt(testNumber)), *(InText.ToString())));
//		FText tempText = FText::FromString(FString::Printf(TEXT("Via BLUEPRINT% s"), *(ft_TheStorySofar.ToString())));
//			MultiLineEditableText_OUT->SetText(tempText);
//}
//
//void UAccessPanelInterface::NativeConstruct()
//{
//	// Do some custom setup
//
//	// Call the Blueprint "Event Construct" node
//	Super::NativeConstruct();
//}
//
//void UAccessPanelInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	// Make sure to call the base class's NativeTick function
//	Super::NativeTick(MyGeometry, InDeltaTime);
//
//	// Do your custom tick stuff here
//	if (haveAddedEditableText_IN_Delegate == false) // TEMP bad way to add the delegate
//	{
//		EditableText_IN->SetText(FText::FromString("Type stuff here"));
//		testNumber = 10000; // Test
//		AddDelegates();
//		haveAddedEditableText_IN_Delegate = true;
//	}
//}

