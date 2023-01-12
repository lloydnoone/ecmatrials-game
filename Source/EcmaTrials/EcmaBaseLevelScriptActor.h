// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "EcmaBaseLevelScriptActor.generated.h"

/**
 * 
 */

UCLASS()
class ECMATRIALS_API AEcmaBaseLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	virtual void PawnKilled(APawn* PawnKilled);

protected:
	template<class T>
	TArray<T*> GetActorsToArray();

	template<class T>
	T* GetActorFromArray(TArray<T*> Array, FName Tag);
};

// put these definitions here as in the cpp they cause linking errors
template<class T>
TArray<T*> AEcmaBaseLevelScriptActor::GetActorsToArray()
{
	TArray<T*> ActorArray;
	for (TActorIterator<T> It(GetWorld()); It; ++It)
	{
		T* Actor = *It;
		ActorArray.Add(Actor);
	}

	//check an actor was actually added
	if (ActorArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Didnt find any Actors for array"));
	}

	return ActorArray;
}

template<class T>
T* AEcmaBaseLevelScriptActor::GetActorFromArray(TArray<T*> Array, FName Tag)
{
	for (T* Actor : Array)
	{
		if (Actor->ActorHasTag(Tag)) return Actor;
	}

	//if non found, log it out and return null
	UE_LOG(LogTemp, Warning, TEXT("No actor found with %s tag"), *Tag.ToString())
		return nullptr;
}
