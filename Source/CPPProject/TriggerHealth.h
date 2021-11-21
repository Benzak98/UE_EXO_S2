// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPPProjectCharacter.h"
#include "TriggerHealth.generated.h"

UCLASS()
class CPPPROJECT_API ATriggerHealth : public AActor
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	ATriggerHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UFUNCTION()
	void TimerFunction();

	UPROPERTY(EditAnywhere)
	int Damage = 20;

	UPROPERTY(VisibleAnywhere)
	ACPPProjectCharacter* Player;

	FTimerHandle TimerHandle;
};
