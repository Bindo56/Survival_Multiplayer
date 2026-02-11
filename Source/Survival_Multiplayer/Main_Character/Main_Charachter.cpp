// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Charachter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Survival_Multiplayer/CharacterComponent/CombatComponent.h"
#include "Survival_Multiplayer/HUD/OverheadWidget.h"
#include "Survival_Multiplayer/Weapon/Weapon.h"


// Sets default values
AMain_Character::AMain_Character()
{
	PrimaryActorTick.bCanEverTick = true;
 	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom-> SetupAttachment(GetMesh());
	CameraBoom-> TargetArmLength = 600;
	CameraBoom-> bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera -> SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera -> bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement() -> bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget -> SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement() -> NavAgentProps.bCanCrouch = true;
}


void AMain_Character::BeginPlay()
{
	Super::BeginPlay();
	
}


void AMain_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}*/

}

// Called to bind functionality to input
void AMain_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController-> GetLocalPlayer()))
		{
			Subsystem-> AddMappingContext(InputMapping,0);
		//	Subsystem-> AddMappingContext(LookMapping,1);
		}
	}
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input-> BindAction(MoveAction,ETriggerEvent::Triggered,this,&AMain_Character::Move);
		Input-> BindAction(LookAction,ETriggerEvent::Triggered,this,&AMain_Character::Look);
		Input-> BindAction(JumpAction,ETriggerEvent::Triggered,this,&AMain_Character::Jump);
		Input-> BindAction(InteractAction,ETriggerEvent::Triggered,this,&AMain_Character::Interact);
		Input-> BindAction(CrouchingAction,ETriggerEvent::Triggered,this,&AMain_Character::CrouchButtonPressed);
		Input-> BindAction(AimAction,ETriggerEvent::Started,this,&AMain_Character::AimButtonPressed);
		Input-> BindAction(AimAction,ETriggerEvent::Completed,this,&AMain_Character::AimButtonReleased);
	}
}

void AMain_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMain_Character,OverlappingWeapon,COND_OwnerOnly);
}

void AMain_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}

void AMain_Character::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		// Get forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement input
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void AMain_Character::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
		
	}
}

void AMain_Character::Jump()
{
	ACharacter::Jump();
}

void AMain_Character::Interact() //server pickup who's owns the server or created
{
	if (Combat)
	{
		if (HasAuthority())
		{
		     Combat->EquipWepon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void AMain_Character::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
	   UnCrouch();	
	}
	else
	{
	  Crouch();
	}
}

void AMain_Character::AimButtonPressed()
{
	if (Combat)
	{
		Combat -> SetAiming(true);
	}
}

void AMain_Character::AimButtonReleased()
{
	if (Combat)
	{
		Combat -> SetAiming(false);
	}
}


void AMain_Character::ServerEquipButtonPressed_Implementation()  //server RPC for Client pickup Gun
{
	if (Combat)
	{
		Combat->EquipWepon(OverlappingWeapon);
	}
	
}

void AMain_Character::OnRep_OverlappingWeapon(AWeapon* LastWeapon) const
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}



void AMain_Character::SetOverLappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool AMain_Character::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AMain_Character::IsAiming()
{
	return (Combat &&  Combat->bAiming);
}



