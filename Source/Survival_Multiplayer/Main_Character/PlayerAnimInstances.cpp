// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstances.h"
#include "Main_Charachter.h"
#include "NetworkMessage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Survival_Multiplayer/Weapon/Weapon.h"


void UPlayerAnimInstances::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Main_Character = Cast<AMain_Character>(TryGetPawnOwner());
}

void UPlayerAnimInstances::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Main_Character == nullptr)
	{
		Main_Character = Cast<AMain_Character>(TryGetPawnOwner());
	}
	if (Main_Character == nullptr)
	{
		return;
	}

	FVector Velocity = Main_Character->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = Main_Character->GetCharacterMovement()->IsFalling();

	bISAccelerating = Main_Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = Main_Character-> IsWeaponEquipped();
	EquippedWeapon = Main_Character->GetEquippedWeapon();
	bIsCrouched = Main_Character -> bIsCrouched;
	bIsAiming = Main_Character->IsAiming();
	TurningInPlace = Main_Character->GetTurningInPlace();

	//Offset Yaw for strafing
	FRotator AimRotation = Main_Character -> GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Main_Character -> GetVelocity());
	FRotator DeltaRot  = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation , AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation , DeltaRot , DeltaSeconds, 6.f);
    YawOffset = DeltaRot.Yaw;
	
	CharacterRotationlastFrame = CharacterRotation;
	CharacterRotation = Main_Character-> GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::  NormalizedDeltaRotator(CharacterRotation , CharacterRotationlastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean , Target, DeltaSeconds,6.f);
    Lean = FMath::Clamp(Interp , -90.f , 90.f);

	Ao_Yaw = Main_Character-> GEtAO_Yaw();
	AO_Pitch = Main_Character -> GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon &&  EquippedWeapon-> getWeaponMesh() && Main_Character->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->getWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		Main_Character-> GetMesh()-> TransformToBoneSpace(FName("LeftHand"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		

		WeaponPos_Implementation();

		//if (Main_Character-> IsLocallyControlled())
		/*{
			//isLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon-> getWeaponMesh()-> GetSocketTransform(FName("RightHand"),RTS_World);
			FRotator LookRot = UKismetMathLibrary::FindLookAtRotation( RightHandTransform.GetLocation(),Main_Character->GetHitTarget());

			LookRot.Yaw += 90.f;
			RightHandRotation = LookRot;
		}*/
		
		//RightHandRotation = FRotator(LookRot.Pitch, LookRot.Yaw, 0.f);

		/*FTransform MuzzleTipTransform = EquippedWeapon-> getWeaponMesh() -> GetSocketTransform(FName("muzzle_Flash_Socket"),RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));*/
		//DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f ,FColor::Red);

		//DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation() ,Main_Character->GetHitTarget(),FColor::Orange);
		
	}
	
}

void UPlayerAnimInstances::WeaponPos_Implementation()
{
	if (Main_Character == nullptr || EquippedWeapon == nullptr) return;
	if (EquippedWeapon->getWeaponMesh() == nullptr) return;

	FTransform RightHandTransform = EquippedWeapon-> getWeaponMesh()-> GetSocketTransform(FName("RightHand"),RTS_World);
	
	FVector Target = Main_Character->GetHitTarget();
	// Fallback: If HitTarget is zero (not synced yet), use the character's aim rotation
	if (Target.IsZero())
	{
		const FVector Start = Main_Character->GetActorLocation() + FVector(0.f, 0.f, 60.f); // Approx Eye Height
		Target = Start + Main_Character->GetBaseAimRotation().Vector() * 5000.f;
	}
	
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation( RightHandTransform.GetLocation(), Target);

	LookRot.Yaw += 90.f;
	RightHandRotation = LookRot;
}
