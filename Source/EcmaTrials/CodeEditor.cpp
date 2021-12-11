// Copyright Prestige Games World Wide.


#include "CodeEditor.h"
#include "Components/MultiLineEditableText.h"
#include "Components/EditableText.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HttpService.h"
#include "Interactable.h"
#include "Blueprint/WidgetLayoutLibrary.h"

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
	HttpService->PostCode(PostCode, this);
}

//catch input before its sent to textinput
FReply UCodeEditor::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//return focus to game to automatically fire off commitinputtext
	if (InKeyEvent.GetKey().ToString() == "LeftControl")
	{
		UE_LOG(LogTemp, Warning, TEXT("code editor handling left control"));
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		return FReply::Handled();
	};

	////use the key if its a letter
	//FString Letter = InKeyEvent.GetKey().ToString();
	//if (Letter.Len() == 1)
	//{
	//	// if shift then upper, if not, lower
	//	if (InKeyEvent.IsLeftShiftDown() || InKeyEvent.IsRightShiftDown())
	//	{
	//		Letter.ToUpperInline();
	//	}
	//	else
	//	{
	//		Letter.ToLowerInline();
	//	}
	//}
	//else
	//{
	//	//if not a letter, just update to latest input
	//	SyntaxHighlight->SetText(TextInput->GetText());
	//	return  FReply::Unhandled();
	//}

	//// add current letter to end of old string
	//FString String = TextInput->GetText().ToString() + Letter;
	//FText Text = FText::FromString(String);

	////set text and latest character for syntax highlighting
	//SyntaxHighlight->SetText(Text);

	return  FReply::Unhandled();
}

//FReply UCodeEditor::NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent)
//{
//	UE_LOG(LogTemp, Warning, TEXT("from text input: %s"), InCharEvent.GetCharacter());
//
//	return  FReply::Unhandled();
//}

//FString String = TextInput->GetText().ToString() + InCharEvent.GetCharacter();
//FText Text = FText::FromString(String);
//SyntaxHighlight->SetText(Text);

void UCodeEditor::SetOwningInteractable(AInteractable* Interactable)
{
	OwningInteractable = Interactable;
}

int32 UCodeEditor::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!OwningInteractable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Code Editor has no owning interactable"));
		return LayerId;
	}

	// draw line from widget to actor
	FVector2D InteractableLocation;

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetWorld()->GetFirstPlayerController(),
		OwningInteractable->GetActorLocation(),
		InteractableLocation,
		false
	);

	// get widget geometry
	FVector2D ScreenEnd = TextInput->GetCachedGeometry().AbsoluteToLocal(InteractableLocation);

	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	// get top right corner of widget
	FVector2D ScreenStart = TextInput->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(1.f, 0.f));
	UWidgetBlueprintLibrary::DrawLine(Context, ScreenStart, InteractableLocation, FLinearColor::White, true, 2.0f);

	// highlight syntax
	FString RawInputText = TextInput->GetText().ToString();
	FString FormattedString = RawInputText.Replace(TEXT("const"), TEXT("<const>const</>"));
	FText Text = FText::FromString(FormattedString);

	//update syntax highlighter to latest
	SyntaxHighlight->SetText(Text);
	
	return LayerId;
}

void UCodeEditor::ReceiveResponse(FResponse_PostCode Response)
{
	// display response output to player
	if (Response.message == "Test passed after updating")
	{
		FText FormattedText = FText::FromString(Response.message);

		ResponseOutput->SetText(FormattedText);
		ResponseOutput->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		PlayAnimation(SlideIn);
	}
	else if (Response.message == "Test failed")
	{
		FString FomattedString = FString(Response.error.name + "\n" + Response.error.message);
		FText FormattedText = FText::FromString(Response.error.message);

		ResponseOutput->SetText(FormattedText);
		ResponseOutput->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		PlayAnimation(SlideIn);
	}
	else
	{
		//FString LineNumber = Response.error.stack.Mid(6,2).TrimEnd();

		FString FomattedString = FString("Line Number: " + Response.error.LineNumber + "\n" + Response.error.name + "\n" + Response.error.message);
		FText FormattedText = FText::FromString(FomattedString);

		ResponseOutput->SetText(FormattedText);
		ResponseOutput->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		PlayAnimation(SlideIn);
	}

	OwningInteractable->SendResultToSubjectActor(Response.error.name.IsEmpty());
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

