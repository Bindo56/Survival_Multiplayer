// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstances.h"
#include "Main_Charachter.h"
#include "NetworkMessage.h"
#include "GameFramework/CharacterMovementComponent.h"


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

}
