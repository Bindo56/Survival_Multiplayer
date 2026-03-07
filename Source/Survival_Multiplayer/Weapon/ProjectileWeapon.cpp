// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority())
	{
		return;
	}
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket =  getWeaponMesh()-> GetSocketByName(FName("muzzle_Flash_Socket"));

	if (MuzzleFlashSocket)
	{
	   FTransform SocketTransform = MuzzleFlashSocket -> GetSocketTransform((getWeaponMesh()));

		// from muzzle flash socket to hit location frim trace under crosshair
		FVector ToTarget  = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation =  ToTarget.Rotation();
		if (ProjectileClss && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = InstigatorPawn;
			
			UWorld* World = GetWorld();
			if (World)
			{
				World-> SpawnActor<AProjectile>(
					ProjectileClss,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParameters
				);
			}
		}
	}
	
}
