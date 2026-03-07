// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AProjectile::AProjectile()
{
 
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoc"));
	SetRootComponent(CollisionBox);
	CollisionBox-> SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox-> SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Block);


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovement-> bRotationFollowsVelocity = true;
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (TracerClass)
	{
		FTransform SpawnTransform = CollisionBox->GetComponentTransform();

		TracerComponent = GetWorld()->SpawnActor<AActor>(
			TracerClass,
			SpawnTransform
		);

		if (TracerComponent)
		{
			TracerComponent->AttachToComponent(
				CollisionBox,
				FAttachmentTransformRules::KeepWorldTransform
			);
		}
	}

	if (HasAuthority())
	{
		CollisionBox -> OnComponentHit.AddDynamic(this,&AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		MulticastImpactFX(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		SetLifeSpan(0.1f);
	}
}

void AProjectile::MulticastImpactFX_Implementation(FVector_NetQuantize Location, FRotator Rotation)
{
	
	if (ImpactParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactParticles,
			Location,
			Rotation
		);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}

	if (ProjectileMovement) ProjectileMovement->StopMovementImmediately();
	SetActorEnableCollision(false);
	if (TracerComponent) TracerComponent->Destroy();
	SetActorHiddenInGame(true);
}

/*void AProjectile::Destroyed()
{
	Super::Destroyed();
}*/


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}
