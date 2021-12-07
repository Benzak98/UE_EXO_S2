// Copyright Epic Games, Inc. All Rights Reserved.

#define PrintString(String) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,String)

#include "CPPProjectCharacter.h"
#include "CPPProjectGameMode.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// ACPPProjectCharacter

ACPPProjectCharacter::ACPPProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	CurrentItem = nullptr;
	bCanMove = true;
	bInspecting = false;

	//ProjectileSpawn = this->Children;
}

void ACPPProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACPPProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PickStart = GetCapsuleComponent()->GetComponentLocation();
	PickForwardVector = GetCapsuleComponent()->GetForwardVector();
	PickEnd = ((PickForwardVector * 200.0f) + PickStart);

	DrawDebugLine(GetWorld(), PickStart, PickEnd, FColor::Green, false, 1, 0, 1);

	if(!bHoldingItem)
	{
		if(GetWorld()->LineTraceSingleByChannel(PickHit, PickStart, PickEnd, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParams))
		{
			if(PickHit.GetActor()->GetClass()->IsChildOf(APickup::StaticClass()))
			{
				CurrentItem = Cast<APickup>(PickHit.GetActor());
			}
		}
		else
		{
			CurrentItem = nullptr;
		}
	}
	
	if(bInspecting)
	{
		if(bHoldingItem)
		{
			HoldingComponent->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f));
			CurrentItem->RotateActor();
		}
	}
	else
	{
		if(bHoldingItem)
		{
			HoldingComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ACPPProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACPPProjectCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACPPProjectCharacter::CrouchEnd);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACPPProjectCharacter::Fire);

	//PlayerInputComponent->BindAction("GetObject", IE_Pressed, this, &ACPPProjectCharacter::GetObject);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ACPPProjectCharacter::OnAction);

	PlayerInputComponent->BindAction("Inspect", IE_Pressed, this, &ACPPProjectCharacter::OnInspect);
	PlayerInputComponent->BindAction("Inspect", IE_Released, this, &ACPPProjectCharacter::OnInspectReleased);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPPProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPPProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACPPProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACPPProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACPPProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACPPProjectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACPPProjectCharacter::OnResetVR);
}


void ACPPProjectCharacter::OnResetVR()
{
	// If CPPProject is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in CPPProject.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACPPProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ACPPProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ACPPProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACPPProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACPPProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && bCanMove)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACPPProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && bCanMove)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACPPProjectCharacter::CrouchStart()
{
	Crouch();
}

void ACPPProjectCharacter::CrouchEnd()
{
	UnCrouch();
}

void ACPPProjectCharacter::Fire()
{
	if (ProjectileClass)
	{
		// Transform MuzzleOffset from camera space to world space.
		FVector const MuzzleLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f);
		
		// Skew the aim to be slightly upwards.
		FRotator const MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			ATPSProjectile* Projectile = World->SpawnActor<ATPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector const LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

/*void ACPPProjectCharacter::GetObject()
{
	FVector Start = GetCapsuleComponent()->GetComponentLocation();
	FVector ForwardVector = GetCapsuleComponent()->GetForwardVector() + FVector(0, 0, -0.5f);
	FVector End = ((ForwardVector * 300.0f) + Start);
	FHitResult Hit;
	FCollisionShape Sphere;
	Sphere.SetSphere(50.0f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 1, 0, 1);
	if(GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Sphere, DefaultComponentQueryParams, DefaultResponseParams))
	{
		if(Hit.GetActor()->GetClass()->IsChildOf())
		{
			PrintString("Sa cast");
		}
	}
}*/

void ACPPProjectCharacter::OnAction()
{
	if(CurrentItem && !bInspecting)
	{
		ToggleItemPickup();
	}
}

void ACPPProjectCharacter::OnInspect()
{
	if(bHoldingItem)
	{
		LastRotation = GetControlRotation();
		ToggleMovement();
	}
	else
	{
		bInspecting = true;
	}
}

void ACPPProjectCharacter::OnInspectReleased()
{
	if(bInspecting && bHoldingItem)
	{
		GetController()->SetControlRotation(LastRotation);
		ToggleMovement();
	}
	else
	{
		bInspecting = false;
	}
}

void ACPPProjectCharacter::ToggleMovement()
{
	bCanMove = !bCanMove;
	bInspecting = !bInspecting;
	CameraBoom->bUsePawnControlRotation = !CameraBoom->bUsePawnControlRotation;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
	
}

void ACPPProjectCharacter::ToggleItemPickup()
{
	if(CurrentItem)
	{
		bHoldingItem = !bHoldingItem;
		CurrentItem->Pickup();
		CurrentItem->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		
		if(!bHoldingItem)
		{
			CurrentItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentItem = nullptr;
		}
	}
}

void ACPPProjectCharacter::SetHealth(int Damage) 
{
	Health -= Damage;

	if (Health >= 100)
		Health = 100;
	else if (Health < 0)
		Health = 0;
}

void ACPPProjectCharacter::Ragdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [this]()
	{
		Destroy();
	}, DestroyDelay, 0);
}

void ACPPProjectCharacter::Die()
{
	Ragdoll();
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [this]()
	{
		AGameModeBase* GM = GetWorld()->GetAuthGameMode();
		if (ACPPProjectGameMode* GameMode = Cast<ACPPProjectGameMode>(GM))
		{
			GameMode->OnPlayerKilled(GetController());
		}
	}, SpawnDelay, 0);
}

