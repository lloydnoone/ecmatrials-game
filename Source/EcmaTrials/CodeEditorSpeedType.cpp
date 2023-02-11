// Copyright MacroHard Systems.


#include "CodeEditorSpeedType.h"
#include "Components/MultiLineEditableText.h"
#include "EcmaEnemyCharacter.h"
#include "CodeEditorComponent.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

bool UCodeEditorSpeedType::Initialize()
{
	UUserWidget::Initialize();

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
	if (!RequiredText)
	{
		UE_LOG(LogTemp, Warning, TEXT("RequiredText is nullptr"));
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
	TextInput->OnTextCommitted.AddDynamic(this, &UCodeEditorSpeedType::DelegateCommitInputText);
	TextInput->OnTextChanged.AddDynamic(this, &Super::DelegateOnTextChanged);

	//set required text
	RequiredText->SetText(FText::FromString(CodeString));
	return true;
}

void UCodeEditorSpeedType::DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType)
{
	// if users input matches required code
	if (TextInput->GetText().ToString() == RequiredText->GetText().ToString())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), CorrectSound);

		// kill that enemy
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		APawn* PlayerPawn = PlayerController->GetPawn();
		GetOwningActor()->TakeDamage(100.f, FDamageEvent(), PlayerController, PlayerPawn);
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		DisplayOutput("Incorrect!", true);
	}
}

void UCodeEditorSpeedType::SetRequiredText(FString String)
{
	//if widget has been created, set text immediately
	if (RequiredText)
	{
		RequiredText->SetText(FText::FromString(String));
		return;
	}
	
	// if not, save the code string which will be used in Init
	CodeString = String;
}