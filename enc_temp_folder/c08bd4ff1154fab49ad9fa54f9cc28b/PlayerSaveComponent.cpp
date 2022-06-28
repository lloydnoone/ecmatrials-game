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
	PlayerSaveData->ForceFieldMap.Emplace(ForceField->Tags[0].ToString(), ForceField->IsHidden());
	
	for (auto& Elem : PlayerSaveData->ForceFieldMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForceFieldMap key: %s"), *Elem.Key);
		UE_LOG(LogTemp, Warning, TEXT("ForceFieldMap value: %b"), Elem.Value);
	}

	UGameplayStatics::SaveGameToSlot(PlayerSaveData, PlayerSaveData->PlayerSlotName, 0);
}

//void UPlayerSaveComponent::LoadForceFieldStatus(TArray<AForceField*> ForceFields)
//{
//	for (auto& Elem : PlayerSaveData->ForceFieldMap)
//	{
//		// get corresponding force field in level
//		for (AForceField* ForceField : ForceFields)
//		{
//			if (ForceField->ActorHasTag(FName(Elem.Key)))
//			{
//				// activate/deactivate depending on saved status
//				ForceField->TestResults(Elem.Value);
//			}
//			else
//			{
//				//if non found, log it out and return null
//				UE_LOG(LogTemp, Warning, TEXT("No actor found with %s tag"), *Elem.Key);
//			}
//		}
//	}
//}

void UPlayerSaveComponent::LoadForceFieldStatus(AForceField* ForceField)
{
	UE_LOG(LogTemp, Warning, TEXT("ForceFieldMap num: %i"), PlayerSaveData->ForceFieldMap.Num());
	for (auto& Elem : PlayerSaveData->ForceFieldMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForceFieldMap key: %s"), *Elem.Key);
		UE_LOG(LogTemp, Warning, TEXT("ForceFieldMap value: %b"), Elem.Value);
	}

	//for (auto& Elem : PlayerSaveData->ForceFieldMap)
	//{
	//	if (ForceField->ActorHasTag(FName(*Elem.Key)))
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Found matching tag"));
	//		// activate/deactivate depending on saved status
	//		ForceField->TestResults(Elem.Value);
	//	}
	//}
}

