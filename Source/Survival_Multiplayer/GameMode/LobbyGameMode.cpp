// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	 int NumberOfPlayer =  GameState.Get()-> PlayerArray.Num();

	if (NumberOfPlayer == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World -> ServerTravel(FString("/Game/Maps/Lvl_ThirdPerson?listen"));
		}
	}
}
