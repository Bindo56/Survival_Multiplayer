// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"
#include "Projectile.generated.h"

UCLASS()
class SURVIVAL_MULTIPLAYER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	//virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
	class AActor* TracerComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TracerClass;

	UPROPERTY(EditAnywhere,Category = "Effects")
	UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactFX(FVector_NetQuantize Location, FRotator Rotation);

public:
	virtual void Tick(float DeltaTime) override;

};
