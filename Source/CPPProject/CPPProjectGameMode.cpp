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


void ACPPProjectGameMode::OnPlayerKilled(AController* Controller)
{
	FVector Location = FVector(-400.0f, 50.0f, 200.0f);
	if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, FRotator::ZeroRotator))
	{
		Controller->Possess(Pawn);
	}
}
