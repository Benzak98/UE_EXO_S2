// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPPProjectGameMode.generated.h"

UCLASS(minimalapi)
class ACPPProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACPPProjectGameMode();

	UFUNCTION()
	void OnPlayerKilled(AController* Controller);

};



