// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpZoneSpeed.generated.h"

class UBoxComponent;

UCLASS()
class SURVIVAL_MULTIPLAYER_API APowerUpZoneSpeed : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUpZoneSpeed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 🔲 Trigger Zone
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	//  Buff Settings
	UPROPERTY(EditAnywhere, Category = "Buff")
	float SpeedMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Buff")
	float BuffDuration = 10.0f;

	// 🔁 Overlap Functions
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	// Buff logic
	void ApplySpeedBuff(class AMain_Character* Player);
	void ResetSpeed(class AMain_Character* Player);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
