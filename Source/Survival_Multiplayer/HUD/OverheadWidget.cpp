// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"


#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText-> SetText(FText::FromString((TextToDisplay)));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"),*Role);
	SetDisplayText(LocalRoleString);
}

void UOverheadWidget::SetBuffTimer(float TimeRemaining)
{
	if (BuffText)
	{
		FString TimerText = FString::Printf(TEXT("Speed Buff: %.1f"), TimeRemaining);
		BuffText->SetText(FText::FromString(TimerText));
	}
	if (TimeRemaining == 0)
	{
		BuffText->SetText(FText::GetEmpty());
	}
}

void UOverheadWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RemoveFromParent();
}

