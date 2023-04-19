// Copyright MacroHard Systems.


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
#include "CodeEditorSceneComponent.h"
#include "EcmaEnemyCharacter.h"
#include "Sound/SoundCue.h"

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

	if (!CorrectSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("correct sound is nullptr"));
		return false;
	}

	if (!ErrorSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error sound is nullptr"));
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

void UCodeEditor::SetOwningComponent(USceneComponent* OwningComp)
{
	OwningComponent = OwningComp;
}

USceneComponent* UCodeEditor::GetOwningComponent()
{
	return OwningComponent;
}

int32 UCodeEditor::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!OwningComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Code Editor has no owning Component"));
		return LayerId;
	}

	// draw line from widget to actor
	FVector2D InteractableLocation;

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetWorld()->GetFirstPlayerController(),
		OwningComponent->GetComponentLocation(),
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
	TArray<FString> Bools = { "true","false","undefined","null"};
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

void UCodeEditor::ReceiveResponse(FResponse_PostCode Response)
{
	// display response output to player
	if (Response.message == "Test passed")
	{
		UGameplayStatics::PlaySound2D(GetWorld(), CorrectSound);
		DisplayOutput(Response.message, false);
	}
	else if (Response.message == "Test failed")
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		FString FomattedString = FString(Response.error.name + "\n" + Response.error.message);
		DisplayOutput(FomattedString, true);
	}
	else
	{
		//FString LineNumber = Response.error.stack.Mid(6,2).TrimEnd();
		UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		FString FomattedString = FString("Line Number: " + Response.error.LineNumber + "\n" + Response.error.name + "\n" + Response.error.message);
		DisplayOutput(FomattedString, true);
	}

	//get this owning components editor component and use it to handle results
	UCodeEditorSceneComponent* EditorComp = nullptr;
	TArray<USceneComponent*> ChildComps = OwningComponent->GetAttachChildren();
	if (ChildComps.Num() != 0)
	{
		for (USceneComponent* AttachComp : ChildComps)
		{
			if (AttachComp)
			{
				if (AttachComp->IsA(UCodeEditorSceneComponent::StaticClass()))
				{
					EditorComp = Cast<UCodeEditorSceneComponent>(AttachComp);
					break;
				}
			}
		}
	}

	if (EditorComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("cant find actors editor scene component"));
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

