// Copyright Epic Games, Inc. All Rights Reserved.

#include "EcmaTrials.h"
#include "Modules/ModuleManager.h"

#include "Internationalization/StringTableRegistry.h"
#include "Misc/Paths.h"


void FEcmaTrials::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	// attempt to load strings from CSV
	/*FStringTableRegistry::Get().UnregisterStringTable("IntroTutorialStrings");

	LOCTABLE_FROMFILE_GAME("IntroTutorialStrings", "IntroTutorialStrings", "Localization/UIStringTable.csv");*/
}

void FEcmaTrials::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, EcmaTrials, "EcmaTrials");