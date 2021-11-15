// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPPProjectGameMode.h"
#include "CPPProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACPPProjectGameMode::ACPPProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
