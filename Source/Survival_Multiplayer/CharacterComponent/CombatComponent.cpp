// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Survival_Multiplayer/Main_Character/Main_Charachter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Survival_Multiplayer/Weapon/Weapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character-> GetCharacterMovement() -> MaxWalkSpeed = BaseWalkSpeed;
	}
	
	// ...
}

void UCombatComponent::SetAiming(bool bISAiming)
{
	bAiming = bISAiming;
	ServerSetAiming(bISAiming);
	if (Character)
	{
		Character -> GetCharacterMovement()-> MaxWalkSpeed = bISAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
	
}

void UCombatComponent::ServerSetAiming_Implementation(bool bISAiming)
{
	bAiming = bISAiming;

	if (Character)
	{
		Character -> GetCharacterMovement()-> MaxWalkSpeed = bISAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	 if (EquippedWeapon && Character)
	 {
	 	Character -> GetCharacterMovement() -> bOrientRotationToMovement = false;
	 	Character -> bUseControllerRotationYaw = true;
	 }
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent,EquippedWeapon);
	DOREPLIFETIME(UCombatComponent,bAiming);
}

void UCombatComponent::EquipWepon(class AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}
    EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character-> GetMesh()-> GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
	}
	EquippedWeapon-> SetOwner(Character);

	Character -> GetCharacterMovement() -> bOrientRotationToMovement = false;
	Character -> bUseControllerRotationYaw = true;
	
}
