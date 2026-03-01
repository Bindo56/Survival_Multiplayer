// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "VectorUtil.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Survival_Multiplayer/Main_Character/Main_Charachter.h"

// Sets default values
AWeapon::AWeapon()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh-> SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh-> SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);

	WeaponMesh-> SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	//SetRootComponent(AreaSphere);
	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}



// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
	if (GetLocalRole() == ROLE_Authority)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);;
		AreaSphere-> OnComponentBeginOverlap.AddDynamic(this,&AWeapon::OnSphereOverlap);
		AreaSphere-> OnComponentEndOverlap.AddDynamic(this,&AWeapon::OnSphereEndOverlap);
	}
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon,WeaponState);  //Add #include "Net/UnrealNetwork.h" for network auth
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMain_Character* Character = Cast<AMain_Character>(OtherActor);
	if (Character)
	{
		Character -> SetOverLappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMain_Character* Character = Cast<AMain_Character>(OtherActor);
	if (Character)
	{
		Character -> SetOverLappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere-> SetCollisionEnabled(ECollisionEnabled:: NoCollision);
		break;
	default: ;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	default: ;
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget) const
{
	if (PickupWidget)
	{
		PickupWidget-> SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (MuzzleFlash == nullptr || WeaponMesh == nullptr)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(  //muzzle
	MuzzleFlash,
	WeaponMesh,
	FName("muzzle_Flash_Socket"),
	FVector::ZeroVector,
	FRotator(90.f,0.f,90.f),
	EAttachLocation::SnapToTarget,
	true
	);
}



