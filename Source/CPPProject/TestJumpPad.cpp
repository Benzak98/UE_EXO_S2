// Fill out your copyright notice in the Description page of Project Settings.


#include "TestJumpPad.h"
#include "GameFramework/Character.h"


// Sets default values
ATestJumpPad::ATestJumpPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NomMesh"));
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->OnComponentHit.AddDynamic(this, &ATestJumpPad::OnHit);

}

// Called when the game starts or when spawned
void ATestJumpPad::BeginPlay()
{
	Super::BeginPlay();
	
	GLog->Log("Bonjour");
	// ou "GEnngine" pour tout le moteur
}

// Called every frame
void ATestJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestJumpPad::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) 
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character == nullptr)
		return;

	Character->LaunchCharacter(FVector(0, 0, 1000), false, true);
}

