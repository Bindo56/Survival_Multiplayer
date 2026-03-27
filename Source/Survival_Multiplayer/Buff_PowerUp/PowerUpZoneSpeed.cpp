// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpZoneSpeed.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Survival_Multiplayer/Main_Character/Main_Charachter.h"

// Sets default values
APowerUpZoneSpeed::APowerUpZoneSpeed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	SetRootComponent(CollisionBox);

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void APowerUpZoneSpeed::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APowerUpZoneSpeed::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &APowerUpZoneSpeed::OnOverlapEnd);
	
}

void APowerUpZoneSpeed::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMain_Character* Player = Cast<AMain_Character>(OtherActor);

	if (Player && Player->HasAuthority())
	{
		ApplySpeedBuff(Player);
	}
}

void APowerUpZoneSpeed::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//AMain_Character* Player = Cast<AMain_Character>(OtherActor);

	/*if (Player && HasAuthority())
	{
		ResetSpeed(Player);
	}*/
}

void APowerUpZoneSpeed::ApplySpeedBuff(class AMain_Character* Player)
{
	if (!Player) return;

	UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement();

	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = 900.0f;
		UE_LOG(LogTemp, Warning, TEXT("Speed Buff Applied"));
	}

	//UI
	Player->CurrentBuffTime = BuffDuration;
	Player->GetWorldTimerManager().SetTimer(
	Player->BuffTimerHandle,
	FTimerDelegate::CreateLambda([Player]()
	{
		Player->CurrentBuffTime -= 0.1f;

		if (Player->CurrentBuffTime < 0.f)
		{
			Player->CurrentBuffTime = 0.f;
		}

		Player->Client_UpdateBuffUI(Player->CurrentBuffTime);//cleintSide
		Player->UpdateBuffUI();//server
	}),
	0.1f,
	true);

	//Timer
	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(
		TimerHandle,
		[this, Player]()
		{
			ResetSpeed(Player);
		},
		BuffDuration,
		false
	);
}

void APowerUpZoneSpeed::ResetSpeed(class AMain_Character* Player)
{
	if (!Player) return;

	UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement();

	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = 300.f;

		UE_LOG(LogTemp, Warning, TEXT("Speed Buff Removed"));
	}
}

// Called every frame
void APowerUpZoneSpeed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

