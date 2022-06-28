// Copyright Prestige Games World Wide.


#include "PlayerSaveComponent.h"
#include "EcmaGameInstance.h"
#include "PlayerSaveData.h"
#include "Kismet/GameplayStatics.h"
#include "ForceField.h"

// Sets default values for this component's properties
UPlayerSaveComponent::UPlayerSaveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerSaveComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerSaveData = Cast<UEcmaGameInstance>(GetWorld()->GetGameInstance())->PlayerSaveData;
	if (!PlayerSaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerSaveData in PlayerSaveComponent is null"));
	}
}


// Called every frame
void UPlayerSaveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void UPlayerSaveComponent::SaveCheckpoint(FString Checkpoint)
{
	PlayerSaveData->SavePointName = Checkpoint;
	UGameplayStatics::SaveGameToSlot(PlayerSaveData, PlayerSaveData->PlayerSlotName, 0);
}

void UPlayerSaveComponent::SaveForceFieldStatus(AForceField* ForceField)
{
	// make sure it has a tag
	if (ForceField->Tags[0].IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("No ForceField tag in SaveForceFieldStatus"));
		return;
	}

	// save tag and status to map
	PlayerSaveData->ForceFieldMap.Emplace(ForceField->Tags[0].ToString(), !ForceField->IsHidden());

	UGameplayStatics::SaveGameToSlot(PlayerSaveData, PlayerSaveData->PlayerSlotName, 0);
}

void UPlayerSaveComponent::LoadForceFieldStatus(AForceField* ForceField)
{
	// see if this forcfield has a saved status
	if (PlayerSaveData->ForceFieldMap.Contains(ForceField->Tags[0].ToString()))
	{
		// if it does, set that status
		bool Status = PlayerSaveData->ForceFieldMap.FindChecked(ForceField->Tags[0].ToString());
		ForceField->TestResults(Status);
	}
}

