// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class AMain_Character;

UCLASS()
class SURVIVAL_MULTIPLAYER_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FrameMesh;

	UPROPERTY(Replicated)
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere)
	float OpenAngle = 90.f;

	UPROPERTY(EditAnywhere)
	float OpenSpeed = 2.f;

	FRotator ClosedRotation;
	FRotator TargetRotation;

public:
	

	void OpenDoor(AMain_Character* Player);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OpenDoor(float Direction);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CloseDoor(float Direction);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
