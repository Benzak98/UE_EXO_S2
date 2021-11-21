// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))
#define PrintString(String) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,String)

#include "TriggerHealth.h"

// Sets default values
ATriggerHealth::ATriggerHealth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ATriggerHealth::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ATriggerHealth::OnOverlapEnd);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NomMesh"));
}

// Called when the game starts or when spawned
void ATriggerHealth::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATriggerHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATriggerHealth::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	// check if Actors do not equal nullptr and that 
	if (OtherActor && (OtherActor != this)) {
		Player = Cast<ACPPProjectCharacter>(OtherActor);
		if (Player == nullptr)
			return;
		// print to screen using above defined method when actor enters trigger box
		print("Overlap Begin");
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ATriggerHealth::TimerFunction, 2.0f, true, 0.1f);
		printFString("Overlapped Actor = %s", *OverlappedActor->GetName());
	}
}

void ATriggerHealth::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this)) {
		// print to screen using above defined method when actor leaves trigger box
		print("Overlap Ended");
		printFString("%s has left the Trigger Box", *OtherActor->GetName());
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}

void ATriggerHealth::TimerFunction()
{
	if (Player == nullptr)
		return;
	Player->SetHealth(Damage);
	if(Player->Health == 0)
	{
		PrintString("Finished Looping");
		GetWorldTimerManager().ClearTimer(TimerHandle);
		Player->Die();
	}
	else
	{
		PrintString("Timer Called");
	}
	printFString("Calls Remaining: %d", Player->Health);
}
