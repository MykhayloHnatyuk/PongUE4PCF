// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGMainBackgroundMenuActor.h"
#include "PNGMainBackgroundWidget.h"
#include "Settings/PNGGameModeMain.h"
#include "Components/WidgetComponent.h"

void APNGMainBackgroundMenuActor::BeginPlay()
{
	Super::BeginPlay();

	APNGGameStateMain* gs = Cast<APNGGameStateMain>(GetWorld()->GetGameState());
	gs->OnGameStateChanged().RemoveAll(this);
	gs->OnGameStateChanged().AddUObject(this, &APNGMainBackgroundMenuActor::OnGameStateChangedHandler);
}

void APNGMainBackgroundMenuActor::InitializeBP(UWidgetComponent* MainWidgetComponent)
{
	mMainMenuWidget = Cast<UPNGMainBackgroundWidget>(MainWidgetComponent->GetUserWidgetObject());
}

#define STATUS_TEXT_WAITING_FOR_PLAYER "let's wait for Player#2"
#define STATUS_TEXT_STARTING_PLAY "get ready"
#define STATUS_TEXT_PLAYING "score %d more to win"
#define STATUS_TEXT_DEFAULT ""
void APNGMainBackgroundMenuActor::OnGameStateChangedHandler(PNGGameState NewState)
{
	ensure(mMainMenuWidget);

	FString Text = STATUS_TEXT_DEFAULT;

	switch (NewState)
	{
	case PNGGameState::gsSetupPlay:
	case PNGGameState::gsWaitingForPlayers:
		Text = STATUS_TEXT_WAITING_FOR_PLAYER;
		break;
	case PNGGameState::gsStartingPlay:
		Text = STATUS_TEXT_STARTING_PLAY;
		break;
	case PNGGameState::gsPlaying:
	{
		int leftToScore = 7;
		Text = *FString::Printf(TEXT(STATUS_TEXT_PLAYING), leftToScore);
	}
		break;
	default:
		Text = STATUS_TEXT_DEFAULT;
		break;
	}

	mMainMenuWidget->UpdateStatus(Text);
}

void APNGMainBackgroundMenuActor::OnScoreChangedHandler(int Player1Score, int Player2Score)
{
	ensure(mMainMenuWidget);

	mMainMenuWidget->UpdateScore(Player1Score, Player2Score);
}
