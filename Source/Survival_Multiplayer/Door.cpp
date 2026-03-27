// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Survival_Multiplayer/Main_Character/Main_Charachter.h"
#include "Net/UnrealNetwork.h"

class AMain_Character; 
// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true); 

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>("Frame");
	SetRootComponent(FrameMesh);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	DoorMesh->SetupAttachment(FrameMesh);

}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, bIsOpen);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	ClosedRotation = DoorMesh->GetRelativeRotation();
	
}


void ADoor::Multicast_OpenDoor_Implementation(float Direction)
{
	//UE_LOG(LogTemp, Warning, TEXT("Multicast called For Door"));
	
	if (bIsOpen) return;

	bIsOpen = true;

	TargetRotation = ClosedRotation + FRotator(0.f, Direction * OpenAngle, 0.f);
	DoorMesh->SetRelativeRotation(TargetRotation);
}

void ADoor::OpenDoor(AMain_Character* Player)
{
	if (!Player) return;
	if (!HasAuthority()) return;

	FVector DoorForward = GetActorForwardVector();
	FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	float Dot = FVector::DotProduct(DoorForward, ToPlayer);
	float Direction = (Dot > 0) ? 1.f : -1.f;

	if (bIsOpen)
	{
		Multicast_CloseDoor(Direction); //  CLOSE
	}
	else
	{
		Multicast_OpenDoor(Direction);  //  OPEN
	}
}

void ADoor::Multicast_CloseDoor_Implementation(float Direction)
{
	//UE_LOG(LogTemp, Warning, TEXT("Multicast Close Door"));

	if (!bIsOpen) return;

	bIsOpen = false;

	TargetRotation = ClosedRotation + FRotator(0.f, Direction * 0, 0.f);
	DoorMesh->SetRelativeRotation(TargetRotation);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (bIsOpen)
	{
		FRotator Current = DoorMesh->GetRelativeRotation();

		FRotator NewRot = FMath::RInterpTo(Current, TargetRotation, DeltaTime, OpenSpeed);

		DoorMesh->SetRelativeRotation(NewRot);
	}*/

}

