// Copyright MacroHard Systems

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_Staggered.generated.h"

class AEcmaEnemyCharacter;

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UBTService_Staggered : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_Staggered();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	AEcmaEnemyCharacter* OwnerPawn = nullptr;

	float AnimLength = 0.0f;
};
