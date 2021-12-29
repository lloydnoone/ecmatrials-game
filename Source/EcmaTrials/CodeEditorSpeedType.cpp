// Copyright Prestige Games World Wide.


#include "CodeEditorSpeedType.h"
#include "Components/MultiLineEditableText.h"
#include "EcmaEnemyCharacter.h"
#include "CodeEditorComponent.h"
#include "Components/TextBlock.h"

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

	//add function to onTextCommitted
	TextInput->OnTextCommitted.AddDynamic(this, &UCodeEditorSpeedType::DelegateCommitInputText);

	//get required text
	RequiredText->SetText(FText::FromString(GetActorsEditorComponent()->GetRequiredText()));
	RequiredText->SetText(FText::FromString(GetActorsEditorComponent()->GetRequiredText()));
	return true;
}

void UCodeEditorSpeedType::DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType)
{
	if (TextInput->GetText().ToString() == RequiredText->GetText().ToString())
	{
		GetOwningActor()->TakeDamage(100.f, FDamageEvent(), GetWorld()->GetFirstPlayerController(), GetWorld()->GetFirstPlayerController()->GetPawn());
		DisplayOutput("Correct! killed an npc", false);
	}
	else
	{
		DisplayOutput("Incorrect!", true);
	}
}