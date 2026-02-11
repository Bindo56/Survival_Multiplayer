// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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

	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();
	void Interact();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	

private:
	UPROPERTY(VisibleAnywhere,Category = Camera)
	class USpringArmComponent*  CameraBoom;

	UPROPERTY(VisibleAnywhere , Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	/**
	 * 
	 */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon) const;

	 UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UFUNCTION(Server,Reliable)  //for RPC Call
	void ServerEquipButtonPressed();
public:
	//getter and setter
	 void SetOverLappingWeapon(AWeapon* Weapon);
	 bool IsWeaponEquipped();
	bool IsAiming();
	
};
