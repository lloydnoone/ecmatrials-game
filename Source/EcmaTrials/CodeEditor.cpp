// Copyright Prestige Games World Wide.


#include "CodeEditor.h"
#include "Components/MultiLineEditableText.h"
#include "Components/EditableText.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HttpService.h"
#include "Interactable.h"
#include "Blueprint/WidgetLayoutLibrary.h"

bool UCodeEditor::Initialize()
{
	Super::Initialize();

	if (!TextInput)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextInput is nullptr"));
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
	HttpService->PostCode(PostCode);
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

	return  FReply::Unhandled();
}

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

	FVector2D InteractableLocation;

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetWorld()->GetFirstPlayerController(),
		OwningInteractable->GetActorLocation(),
		InteractableLocation,
		false
	);

	FVector2D ScreenEnd = TextInput->GetCachedGeometry().AbsoluteToLocal(InteractableLocation);

	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FVector2D ScreenStart = TextInput->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(1.f, 0.f));
	UWidgetBlueprintLibrary::DrawLine(Context, ScreenStart, InteractableLocation, FLinearColor::White, true, 2.0f);

	return LayerId;
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

