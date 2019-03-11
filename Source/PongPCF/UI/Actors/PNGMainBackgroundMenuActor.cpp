// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGMainBackgroundMenuActor.h"
#include "PNGMainBackgroundWidget.h"
#include "Settings/PNGGameModeMain.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine.h"

void APNGMainBackgroundMenuActor::BeginPlay()
{
	Super::BeginPlay();

	APNGGameStateMain* gs = Cast<APNGGameStateMain>(GetWorld()->GetGameState());
	gs->OnGameStateChanged().RemoveAll(this);
	gs->OnGameStateChanged().AddUObject(this, &APNGMainBackgroundMenuActor::OnGameStateChangedHandler);
	gs->OnGameScoreChanged().RemoveAll(this);
	gs->OnGameScoreChanged().AddUObject(this, &APNGMainBackgroundMenuActor::OnGameScoreChangedHandler);
}

void APNGMainBackgroundMenuActor::InitializeBP(UWidgetComponent* MainWidgetComponent)
{
	mMainMenuWidget = Cast<UPNGMainBackgroundWidget>(MainWidgetComponent->GetUserWidgetObject());
}

#define STATUS_TEXT_WAITING_FOR_PLAYER "let's wait for Player#2"
#define STATUS_TEXT_STARTING_PLAY "get ready"
#define STATUS_TEXT_PLAYING "score %d more to win"
#define STATUS_TEXT_WON "yeah! let's do it again"
#define STATUS_TEXT_LOSE "you lost :( let's try again"
#define STATUS_TEXT_DEFAULT ""
void APNGMainBackgroundMenuActor::OnGameStateChangedHandler(PNGGameState NewState)
{
	// Let's update our status text based on a current game state.

	ensure(mMainMenuWidget);

	FString Text = STATUS_TEXT_DEFAULT;

	auto GetLocalPlayerScore = [this]()->int
	{
		auto pc = UGameplayStatics::GetPlayerController(this, 0);
		return pc->PlayerState->Score;
	};

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
		int currentScore = GetLocalPlayerScore();
		int leftToScore = TARGET_GAME_SCORE_TO_WIN - currentScore;
		Text = *FString::Printf(TEXT(STATUS_TEXT_PLAYING), leftToScore);
	}
		break;
	case PNGGameState::gsFinished:
	{
		int localPlayerSocre = GetLocalPlayerScore();
		bool bWeWon = localPlayerSocre >= TARGET_GAME_SCORE_TO_WIN;
		Text = bWeWon ? STATUS_TEXT_WON : STATUS_TEXT_LOSE;
	}
		break;
	default:
		Text = STATUS_TEXT_DEFAULT;
		break;
	}

	mMainMenuWidget->UpdateStatus(Text);
}

void APNGMainBackgroundMenuActor::OnGameScoreChangedHandler(int Player1Score, int Player2Score)
{
	ensure(mMainMenuWidget);

	mMainMenuWidget->UpdateScore(Player1Score, Player2Score);
}
