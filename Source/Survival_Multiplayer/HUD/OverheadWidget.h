// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVAL_MULTIPLAYER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BuffText;

	void SetDisplayText(FString TextToDisplay);
	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);
	
public:
	void SetBuffTimer(float TimeRemaining);

protected:
	virtual void NativeDestruct() override;
	
};
