// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CodeEditor.generated.h"

class UMultiLineEditableText;
class UTextBlock;
class AHttpService;
class UCodeEditorComponent;
class URichTextBlock;
struct FResponse_PostCode;

UCLASS()
class ECMATRIALS_API UCodeEditor : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMultiLineEditableText* TextInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	URichTextBlock* SyntaxHighlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ResponseOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* InfoText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ToSubmit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ToExit;

	// get access to this anim to trigger in C++
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* SlideIn;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* PopOut;

	UFUNCTION()
	virtual void DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType);

	// catch keyboard presses before text input handles them
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// virtual FReply NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent) override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetOwningActor(AActor* Actor);

	void SetRequestUrl(FString Url);

	AActor* GetOwningActor();

	UCodeEditorComponent* GetActorsEditorComponent();

	void ReceiveResponse(FResponse_PostCode Response);

	void DisplayOutput(FString String, bool IsError);

	void AddEditorToScreen();

private:

	FString RequestUrl = "";

	FString ErrorString = "";

	AHttpService* HttpService;

	AActor* OwningActor;

	void GetHttpService();

	//code below was an attempt to fire off onsubmittext, might be usefull

	//UPROPERTY(meta = (BindWidget))
	//class UMultiLineEditableText* MultiLineEditableText_OUT;

	//UPROPERTY(meta = (BindWidget))
	//class UEditableText* EditableText_IN;

	//FText ft_TheStorySofar;
	//int testNumber = 0;
	//bool haveAddedEditableText_IN_Delegate = false; // TEMP bad way to add the delegate after killing the editor doing it other ways.

	//UAccessPanelInterface(const FObjectInitializer& ObjectInitializer);

	//void AddDelegates();

	//// Do it via a C++ delegate
	//void DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType);

	//// For test because I have not yet got the commit multi-delegate to call my function DelegateCommitInputText()
	//UFUNCTION(BlueprintCallable, Category = "fairyTale")
	//void BlueprintCommitInputText(const FText& InText);

	//// Optionally override the Blueprint "Event Construct" event
	//virtual void NativeConstruct() override;

	//// Optionally override the tick event
	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
