// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterType.h"
#include "Main_Charachter.generated.h"

UCLASS()
class SURVIVAL_MULTIPLAYER_API AMain_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AMain_Character();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);

protected:
	virtual void BeginPlay() override;
	//Input Region
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* LookMapping;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* CrouchingAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* FireAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* GrabMagAction;

	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	virtual void Jump() override;
	void Interact();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffSet(float DeltaTime);
	void FireButtonPressed();
	void FireButtonReleased();
	void OnGrabPressed();
	void OnGrabReleased();
	
	

private:
	UPROPERTY(VisibleAnywhere,Category = Camera)
	class USpringArmComponent*  CameraBoom;

	UPROPERTY(VisibleAnywhere , Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	//Door
	UFUNCTION(Server, Reliable)
	void Server_TryOpenDoor(class ADoor* Door);
	//

	/**
	 * 
	 */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon) const;

	 UPROPERTY(EditAnywhere)
	class UCombatComponent* Combat;

	UFUNCTION(Server,Reliable)  //for RPC Call
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	FVector Impluse;

	//SpeedUp
public:
	FTimerHandle BuffTimerHandle;
	UPROPERTY(ReplicatedUsing = OnRep_BuffTime)
	float CurrentBuffTime = 0.f;
	void UpdateBuffUI();

	UFUNCTION()
	void OnRep_BuffTime();
	
	UFUNCTION(Client, Reliable)
	void Client_UpdateBuffUI(float TimeRemaining);

	//

	//MagazineMech
	bool bHoldingMag = false;
	FVector2D AccumulatedMouseDelta;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere,Category = Combat)
	class UAnimMontage* FireWeaponMontage;
public:
	//getter and setter
	 void SetOverLappingWeapon(AWeapon* Weapon);
	 bool IsWeaponEquipped();
	 bool IsAiming();
	 FORCEINLINE float GEtAO_Yaw() const {return  AO_Yaw;}
	 FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	AWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return  TurningInPlace;}

	bool ShouldUseAimingLocomotion() const;
	FVector GetHitTarget() const;

	FORCEINLINE  UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	
};
