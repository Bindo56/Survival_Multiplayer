// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Survival_Multiplayer/Main_Character/Main_Charachter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Survival_Multiplayer/Weapon/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Survival_Multiplayer/PlayerController/MainPlayerController.h"
#include "Survival_Multiplayer/HUD/MainCharacterHUD.h"
#include "Windows/WindowsApplication.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;

	
	

	// ...
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent,EquippedWeapon);
	DOREPLIFETIME(UCombatComponent,bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, HitTarget, COND_SkipOwner);
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AMain_Character>(GetOwner());
	PrimaryComponentTick.bCanEverTick = true;                  //not running when done in construture
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;
	/*UE_LOG(LogTemp, Error, TEXT("CombatComponent BeginPlay called"));

	UE_LOG(LogTemp, Error, TEXT("bCanEverTick: %d"), PrimaryComponentTick.bCanEverTick);*/
	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);
	
	
	if (Character)
	{
		Character-> GetCharacterMovement() -> MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character ->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
	
	// ...
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									 FActorComponentTickFunction* ThisTickFunction)
{
	//UE_LOG(LogTemp, Warning, TEXT("Combat Tick Running"));
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	// ...//comment out this to remove the circle debug 
	
	if (Character &&  Character->IsLocallyControlled())
	{
	    FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
	
	if (Character->GetVelocity().Size2D()> 0.f)
	{
     	SetAimmingWhileWalking(true);
	}else
	{
		//SetAiming(false);
	}
		
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	
	if (Character == nullptr || Character->Controller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Null"));
		return;
		
	}

	Controller = Controller == nullptr ?  Cast<AMainPlayerController>(Character -> Controller) : Controller;	//good way to cast
	if (Controller)
	{
		
		HUD = HUD == nullptr ? Cast<AMainCharacterHUD>(Controller -> GetHUD()) : HUD;
		if (HUD)
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD got"));
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairCenter = EquippedWeapon-> CrosshiarCenter;
				HUDPackage.CrosshairTop = EquippedWeapon -> CrosshiarTop;
				HUDPackage.CrosshairBottom = EquippedWeapon -> CrosshiarBottom;
				HUDPackage.CrosshairLeft = EquippedWeapon -> Crosshiarleft;
				HUDPackage.CrosshairRight = EquippedWeapon-> Crosshiarright;
				
			}else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
			}
			//calculate crosshair spread
			FVector2D WalkSpeedRange(0.f,Character -> GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMulti(0.f,1.f);
			FVector Velocity = Character -> GetVelocity();
			Velocity.Z = 0.f;
			
		    CrosshairVelocityFactor= FMath::GetMappedRangeValueClamped(WalkSpeedRange,VelocityMulti,Velocity.Size());

			if (Character -> GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor , 2.25f , DeltaTime , 2.25f);
			}else
			{
				
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor , 0.f , DeltaTime , 30.f);
			}
			
			HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairInAirFactor; 
			
			HUD->SetHUDPackage(HUDPackage);
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("HUDNull"));
		}
	}
}

void UCombatComponent::SetAimmingWhileWalking(bool isWalking)
{
	if (!Character || !EquippedWeapon || !isWalking) return;

	float Speed = Character->GetVelocity().Size2D(); // ignore vertical velocity

	bool bIsWalking = Speed > 0.f;

	bAiming = bIsWalking;

	Character->GetCharacterMovement()->MaxWalkSpeed =
		bAiming ? AimWalkSpeed : BaseWalkSpeed;

}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV , EquippedWeapon->GetZoomedFOV(),DeltaTime,EquippedWeapon->GetZoomInterSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV , DeltaTime,ZoomInterpSpeed);
	}

	if (Character && Character ->GetFollowCamera())
	{
		Character-> GetFollowCamera() -> SetFieldOfView(CurrentFOV);
	}
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

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPrssed = bPressed;

	if (!bAiming)
	{
	   return;
	}
	if (bFireButtonPrssed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult); //get the hit result
		ServerFire(HitResult.ImpactPoint);
	}
	
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	if (!Character || !Character->IsLocallyControlled())
	{
		//UE_LOG(LogTemp, Warning, TEXT("return from the TraceUnderCrosshairs function."));
		return;
	}
	FVector2D ViewportSize;
	if (GEngine && GEngine -> GameViewport) 
	{
		GEngine ->GameViewport-> GetViewportSize(ViewportSize); //get screen center location 
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(    //converting 2D screen space into a 3D ray in the world.
		UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGHT;

		 GetWorld()-> LineTraceSingleByChannel(
		
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		//remove from here
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			DrawDebugSphere(                              //will remove later
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
				);
		}
		//to here  later
	}
	
}


void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	if (Character) //charchter willl is null when not aiming 
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Character is NULL  Montage will not play."));
	}
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
		if (EquippedWeapon->getWeaponMesh())
		{
			EquippedWeapon->getWeaponMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 260.f));
		}
	}
	EquippedWeapon-> SetOwner(Character);

	Character -> GetCharacterMovement() -> bOrientRotationToMovement = false;
	Character -> bUseControllerRotationYaw = true;
	
}
