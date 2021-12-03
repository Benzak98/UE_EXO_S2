// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPPProjectCharacter.h"
#include "Components/BoxComponent.h"
#include "LoadLevel.generated.h"

UCLASS()
class CPPPROJECT_API ALoadLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALoadLevel();

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

	UFUNCTION()
	void Load();

	UFUNCTION()
	void UnLoad();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	ACPPProjectCharacter* Player;

	UPROPERTY(EditAnywhere)
	int LevelCallingManagement;

	UPROPERTY(EditAnywhere)
	FName Level;

};
