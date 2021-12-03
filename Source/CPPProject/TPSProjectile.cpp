// Fill out your copyright notice in the Description page of Project Settings.
#define PrintString(String) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,String)

#include "TPSProjectile.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

// Sets default values
ATPSProjectile::ATPSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if(!RootComponent)
	{
		SpawnProjectile = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
		RootComponent = SpawnProjectile;
	}
	if(!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(10.0f);
		CollisionComponent->SetupAttachment(RootComponent);
	}

	if(!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("[StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere']"));
		if(Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

	if(!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 2000.0f;
		ProjectileMovementComponent->MaxSpeed = 2000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.2f;
	}

	/*static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("[Material'/Game/Materials/M_Projectile.M_Projectile']"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}*/
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;

	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ATPSProjectile::OnHit);
	ProjectileMeshComponent->SetupAttachment(CollisionComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial>DecalMaterial(TEXT("[Material'/Game/Materials/Decals/M_Spatter.M_Spatter']"));
	if (DecalMaterial.Succeeded())
	{
		M_Decal = UMaterialInstanceDynamic::Create(DecalMaterial.Object, DecalMaterial.Object);
		M_Decal->GetMaterial()->SetScalarParameterValueEditorOnly(FName("Frame"), 1);
		PrintString(M_Decal->GetMaterial()->GetName());
	}
	
}

// Called when the game starts or when spawned
void ATPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}
	PrintString(Hit.Normal.Rotation().ToString());
	
	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(Hit.Location, FRotator());
	if(decal)
	{
		decal->SetActorRotation(Hit.Normal.Rotation());
		decal->SetDecalMaterial(M_Decal);
		decal->GetDecal()->DecalSize = FVector(32.0f, 64.0f, 64.0f);
		decal->GetDecal()->SetFadeOut(0.0f, 2.0f, true);
	}
	Destroy();
}


void ATPSProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

