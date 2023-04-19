// Copyright MacroHard Systems


#include "BTService_Staggered.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EcmaEnemyCharacter.h"

UBTService_Staggered::UBTService_Staggered()
{
	NodeName = "Update staggered status";
}

void UBTService_Staggered::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//get AI pawn
	if (!OwnerPawn)
	{
		OwnerPawn = Cast<AEcmaEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
		if (OwnerPawn == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldnt get owner pawn in staggered service."));
		}

	}

	// set anim length
	if (AnimLength == 0.0f)
	{
		AnimLength = OwnerPawn->GetStaggerAnim()->GetPlayLength();
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TEXT("StaggerAnimLength"), AnimLength);
	}
	
	// set staggered status this tick
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), OwnerPawn->IsStaggered());
}