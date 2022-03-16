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

//catch input before its sent to textinput
FReply UCodeEditor::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//return focus to game to automatically fire off commitinputtext
	if (InKeyEvent.GetKey().ToString() == "LeftControl")
	{
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		return FReply::Handled();
	};

	//use the key if its a letter
	FString Letter = InKeyEvent.GetKey().ToString();
	if (Letter.Len() == 1)
	{
		// if shift then upper, if not, lower
		if (InKeyEvent.IsLeftShiftDown() || InKeyEvent.IsRightShiftDown())
		{
			Letter.ToUpperInline();
		}
		else
		{
			Letter.ToLowerInline();
		}
	}
	else
	{
		//if not a letter, just update to latest input
		SyntaxHighlight->SetText(TextInput->GetText());
		return  FReply::Unhandled();
	}

	// add current letter to end of old string
	FString String = TextInput->GetText().ToString() + Letter;
	FText Text = FText::FromString(String);

	//set text and latest character for syntax highlighting
	SyntaxHighlight->SetText(Text);

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

	// highlight syntax
	FString RawInputText = TextInput->GetText().ToString();
	FString FormattedString = RawInputText.Replace(TEXT("const"), TEXT("<const>const</>")).Replace(TEXT("let"), TEXT("<const>let</>"));
	FText Text = FText::FromString(FormattedString);

	//update syntax highlighter to latest
	SyntaxHighlight->SetText(Text);
	
	return LayerId;
}

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

	//get actors editor component and use it to handle results
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
	TextInput->SetKeyboardFocus();
	PlayAnimation(PopOut);
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

