// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Charachter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Survival_Multiplayer/CharacterComponent/CombatComponent.h"
#include "Survival_Multiplayer/HUD/OverheadWidget.h"
#include "Survival_Multiplayer/Weapon/Weapon.h"
#include "PlayerAnimInstances.h"
#include "Survival_Multiplayer/Door.h"


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
	GetCapsuleComponent() -> SetCollisionResponseToChannel(ECC_Camera , ECR_Ignore);
	GetMesh() -> SetCollisionResponseToChannel(ECC_Camera , ECR_Ignore);
	GetCharacterMovement()-> RotationRate = FRotator(0.f,0.f,850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
	
	
}


void AMain_Character::BeginPlay()
{
	Super::BeginPlay();
	
}


void AMain_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffSet(DeltaTime);
	Combat->TickComponent(DeltaTime, LEVELTICK_All, nullptr);

	if (bHoldingMag)
	{
		float DeltaX = 0.f;
		float DeltaY = 0.f;

		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC)
		{
			PC->GetInputMouseDelta(DeltaX, DeltaY);
		}

		AccumulatedMouseDelta += FVector2D(DeltaX, DeltaY);
		AccumulatedMouseDelta = AccumulatedMouseDelta.GetClampedToMaxSize(150.f);

		float Strength = AccumulatedMouseDelta.Size();

		// KEY FIX: threshold
		if (Strength < 20.f)
		{
			Impluse = FVector::ZeroVector; // just drop
			return;
		}

		// Direction
		FVector2D InputDir2D = AccumulatedMouseDelta.GetSafeNormal();
		FVector ThrowDir = FVector(InputDir2D.X, 0.f, -InputDir2D.Y).GetSafeNormal();

		// Strength scaling
		float Normalized = Strength / 150.f;
		float Force = FMath::Lerp(300.f, 1200.f, Normalized);

		Impluse = ThrowDir * Force;
		UE_LOG(LogTemp, Warning, TEXT("Impulse: %s"), *Impluse.ToString());
	}

	
	/*if (bHoldingMag)
	{
		float DeltaX = 0.f;
		float DeltaY = 0.f;
		APlayerController* PC = Cast<APlayerController>(Controller);

		if (PC)
		{
			PC->GetInputMouseDelta(DeltaX, DeltaY);
			UE_LOG(LogTemp, Warning, TEXT("Mouse Delta X: %f Y: %f"), DeltaX, DeltaY);
		}
		AccumulatedMouseDelta += FVector2D(DeltaX, DeltaY);
	}*/

	//UE_LOG(LogTemp, Warning, TEXT("Character Tick"));
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
		Input-> BindAction(FireAction,ETriggerEvent::Started,this,&AMain_Character::FireButtonPressed);
		Input-> BindAction(FireAction,ETriggerEvent::Completed,this,&AMain_Character::FireButtonReleased);
		Input->BindAction(GrabMagAction, ETriggerEvent::Started, this, &AMain_Character::OnGrabPressed);
		Input->BindAction(GrabMagAction, ETriggerEvent::Completed, this, &AMain_Character::OnGrabReleased);
	}
}

void AMain_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMain_Character,OverlappingWeapon,COND_OwnerOnly);
	DOREPLIFETIME(AMain_Character, CurrentBuffTime);
}

void AMain_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}

void AMain_Character::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr  ||Combat-> EquippedWeapon == nullptr)
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		if (Combat->bAiming)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Aiming and moving"));
			float Duration = AnimInstance->Montage_Play(FireWeaponMontage);
			if (Duration > 0.f)
			{
				FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
				AnimInstance->Montage_JumpToSection(SectionName);

				//UE_LOG(LogTemp, Warning, TEXT("Fire montage played. Section: %s"), *SectionName.ToString());
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("Montage failed to play."));
			}
		}
		else if (!Combat->bAiming && ShouldUseAimingLocomotion())
		{
			//UE_LOG(LogTemp, Error, TEXT("Moving not aiming"));
			float Duration = AnimInstance->Montage_Play(FireWeaponMontage);
			if (Duration > 0.f)
			{
				FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
				AnimInstance->Montage_JumpToSection(SectionName);

				//UE_LOG(LogTemp, Warning, TEXT("Fire montage played. Section: %s"), *SectionName.ToString());
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("Montage failed to play."));
			}
		}
	//
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
	if (Combat)
	{
		const bool bIsMoving = !InputVector.IsNearlyZero();
		Combat->SetAimmingWhileWalking(bIsMoving);
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
	if (bIsCrouched)
	{
		UnCrouch();
	}else
	{
	 ACharacter::Jump();
		
	}
}

void AMain_Character::Interact() //server pickup who's owns the server or created
{
	if (Combat == nullptr)
	{
		return;
	}
	FHitResult Hit;
	Combat->TraceUnderCrosshairs(Hit);
	//UE_LOG(LogTemp, Warning, TEXT("Door Detected"));
	ADoor* Door = Cast<ADoor>(Hit.GetActor());
	
	if (Door)
	{
		if (!HasAuthority())
		{
		  Server_TryOpenDoor(Door);
		}else
		{
			Server_TryOpenDoor(Door);
		}
			
		return;
	}


	
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

void AMain_Character::Server_TryOpenDoor_Implementation(ADoor* Door)
{
	if (!Door) return;

	float Distance = FVector::Dist(GetActorLocation(), Door->GetActorLocation());
	if (Distance > 200.f) return;

	// Call normal function on door (NOT RPC)
	Door->OpenDoor(this);
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

void AMain_Character::AimOffSet(float DeltaTime)
{
	if (Combat &&  Combat-> EquippedWeapon == nullptr)
	{
		return;
	}
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement() -> IsFalling();
	if (Speed == 0.f && !bIsInAir) //standing Still Not jumping
	{
		FRotator CurrentAimRotation =  FRotator(0.f ,GetBaseAimRotation().Yaw , 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			   InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
		
	}
	
	if (Speed > 0.f || bIsInAir) //running jumping
	{
		StartingAimRotation = FRotator(0.f ,GetBaseAimRotation().Yaw , 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch  = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())  //doing so that the unsigned number -90 can be maped to 270 beacuse of bitwise network cimpression
	{
		//map pitch from 270 to 360 to [-90 to 0]
		FVector2d InRange(270.f , 360.f);
		FVector2d OutRange(-90.f,0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange,OutRange,AO_Pitch);
	}

}

void AMain_Character::FireButtonPressed()
{
	if (Combat && Combat->EquippedWeapon)
	{
		//UE_LOG(LogTemp, Warning, TEXT("FireButtonPressed"));
		Combat->FireButtonPressed(true);
	}
}

void AMain_Character::FireButtonReleased()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(false);
	}
}

void AMain_Character::OnGrabPressed()
{
	bHoldingMag = true;
	AccumulatedMouseDelta = FVector2D::ZeroVector;

	AWeapon* Weapon = GetEquippedWeapon();
	//UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"));
	if (Weapon)
	{
		Weapon->Server_StartReload(); // spawn + attach
	}
	
}

void AMain_Character::OnGrabReleased()
{
	bHoldingMag = false;

	AWeapon* Weapon = GetEquippedWeapon();
	//UE_LOG(LogTemp, Warning, TEXT("Grab Released"));
	if (Weapon)
	{
		FVector Dir = FVector(
			AccumulatedMouseDelta.X,
			0.f,
			-AccumulatedMouseDelta.Y
		).GetSafeNormal();

		
		//UE_LOG(LogTemp, Warning, TEXT("Released Impluse :  "),Impluse);
		Weapon->Server_RemoveMagazine(Impluse);
	}
}

void AMain_Character::UpdateBuffUI()
{
	if (OverheadWidget) 
	{
		UOverheadWidget* Widget = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());
		//UE_LOG(LogTemp, Warning, TEXT("BufferTime :  "),CurrentBuffTime);
		Widget->SetBuffTimer(CurrentBuffTime);
	}
}

void AMain_Character::OnRep_BuffTime()
{
	UpdateBuffUI();
}

void AMain_Character::Client_UpdateBuffUI_Implementation(float TimeRemaining)
{
	if (OverheadWidget)
	{
		UOverheadWidget* Widget =
			Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());

		if (Widget)
		{
			Widget->SetBuffTimer(TimeRemaining);
		}
	}
}

void  AMain_Character :: TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 50.f)  //turn roatation value
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -50.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw , 0.f,DeltaTime , 5.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f ,GetBaseAimRotation().Yaw , 0.f);
		}
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

AWeapon* AMain_Character::GetEquippedWeapon()
{
	if (Combat == nullptr)
	{
		return nullptr;
	}
	return  Combat->EquippedWeapon;
}

bool AMain_Character::ShouldUseAimingLocomotion() const  //using this bacuse of anim animation using in without aimnimations 
{
	bool bIsMoving =
		GetVelocity().Size2D() > 0.f &&
		GetCharacterMovement() &&
		!GetCharacterMovement()->IsFalling();

	bool bManualAim = Combat && Combat->bAiming;

	return bManualAim || bIsMoving;
}

FVector AMain_Character::GetHitTarget() const
{
	if (Combat == nullptr)
	{
		return FVector();
	}	 
	return Combat->HitTarget;
}



