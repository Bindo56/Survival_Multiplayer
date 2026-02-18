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
		

	}
	
}
