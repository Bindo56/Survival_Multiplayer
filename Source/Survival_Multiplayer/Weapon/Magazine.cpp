// Fill out your copyright notice in the Description page of Project Settings.


#include "Magazine.h"

// Sets default values
AMagazine::AMagazine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>("MagazineMesh");
	SetRootComponent(MagazineMesh);

	MagazineMesh->SetSimulatePhysics(false);
	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AMagazine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagazine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagazine::EnablePhysics(const FVector& Impulse)
{
	
	//UE_LOG(LogTemp, Warning, TEXT("Magazine_ThrowReload"));
	MagazineMesh->SetSimulatePhysics(true);
	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MagazineMesh->AddImpulse(Impulse);
}

