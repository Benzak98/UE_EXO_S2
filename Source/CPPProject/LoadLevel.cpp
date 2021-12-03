// Fill out your copyright notice in the Description page of Project Settings.
#define PrintFString(String) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,String)

#include "LoadLevel.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALoadLevel::ALoadLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ALoadLevel::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ALoadLevel::OnOverlapEnd);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	
}

// Called when the game starts or when spawned
void ALoadLevel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALoadLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALoadLevel::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if(OtherActor && OtherActor != this)
	{
		Player = Cast<ACPPProjectCharacter>(OtherActor);
		if (Player == nullptr)
			return;

		if(LevelCallingManagement == 0)
			Load();
		else if(LevelCallingManagement == 1)
			UnLoad();
	}
}

void ALoadLevel::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if(LevelCallingManagement == 2)
			Load();
		else if(LevelCallingManagement == 3)
			UnLoad();
	}
}

void ALoadLevel::Load()
{
	FLatentActionInfo LatentInfo;
	UGameplayStatics::LoadStreamLevel(this, Level, true, false, LatentInfo);
}

void ALoadLevel::UnLoad()
{
	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevel(this, Level, LatentInfo, false);
}
