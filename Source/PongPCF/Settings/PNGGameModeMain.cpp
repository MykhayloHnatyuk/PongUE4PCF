// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGameModeMain.h"
#include "GameObjects/PNGBall.h"
#include "GameObjects/PNGPlayerStart.h"
#include "GameObjects/PNGGoalZone.h"
#include "Player/PNGPlayerControllerMain.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Engine.h"

#define DEFAULT_BALL_LOCATION FVector::ZeroVector

APNGGameModeMain::APNGGameModeMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void APNGGameModeMain::BeginPlay()
{
	Super::BeginPlay();

	APNGGameStateMain* gs = Cast<APNGGameStateMain>(GetWorld()->GetGameState());
	gs->OnGameStateChanged().RemoveAll(this);
	gs->OnGameStateChanged().AddUObject(this, &APNGGameModeMain::OnGameStateChangedHandler);
	
	ServerRPCSpawnBall();
}

AActor* APNGGameModeMain::ChoosePlayerStart_Implementation(AController* Player)
{	
	AActor* FoundPlayerStart = nullptr;
	UWorld* World = GetWorld();
	const APNGPlayerControllerMain* pngPlayer = Cast<APNGPlayerControllerMain>(Player);
	ensure(pngPlayer);

	for (TActorIterator<APNGPlayerStart> It(World); It; ++It)
	{
		APNGPlayerStart* PlayerStart = *It;
		bool isPlayerOne = pngPlayer->IsPlayerOne();

		if(PlayerStart->IsGoodForPlayerOne() == isPlayerOne)		
		{
			FoundPlayerStart = PlayerStart;
			break;
		}
	}

	if (FoundPlayerStart == nullptr)
	{
		FoundPlayerStart = Super::ChoosePlayerStart(Player);
	}

	return FoundPlayerStart;
}

void APNGGameModeMain::OnGameStateChangedHandler(PNGGameState NewState)
{
	switch (NewState)
	{
	case PNGGameState::gsWaitingForPlayers:
		break;
	case PNGGameState::gsStartingPlay:
		mBall->StopBallAtLocation(DEFAULT_BALL_LOCATION);
		break;
	case PNGGameState::gsPlaying:
		// Game should be started. Let's kick a ball.
		mBall->PushBallInRandomDirection();
		break;
	case PNGGameState::gsExiting:
		ExitLevel();
	default:
		break;
	}
}

void APNGGameModeMain::OnBallHitGoalHandler(APNGGoalZone* GoalZone)
{
	APNGGameStateMain* gs = Cast<APNGGameStateMain>(GetWorld()->GetGameState());

	if(gs->GetCurrentState() != PNGGameState::gsPlaying)
	{
		// Just in case if somehow end up here twice during the same play.
		// We have already counted this score.
		return;
	}

	bool bPlayerOneScored = !GoalZone->IsPlayerOneGoalZone();
	int player1Score = 0;
	int player2Score = 0;

	// Calculate new scores.
	for(auto player : gs->PlayerArray)
	{
		auto controller = Cast<APNGPlayerControllerMain>(player->GetOwner());
		player->Score += controller->IsPlayerOne() == bPlayerOneScored;
		controller->IsPlayerOne() ? player1Score = player->Score : player2Score = player->Score;
	}

	// Update score for everybody.
	gs->UpdateGameScore(player1Score, player2Score);
	
	if(FMath::Max(player1Score, player2Score) >= TARGET_GAME_SCORE_TO_WIN)
	{
		// Game session finished.
		gs->TrySwitchState(PNGGameState::gsFinished);
	}
	else
	{
		// Still no winner. Start next play.
		gs->TrySwitchState(PNGGameState::gsStartingPlay);
	}
}

#define DISCONNECT_COMMAND "disconnect"
#define OPEN_LOBBY_COMMAND "open Lobby"
void APNGGameModeMain::ExitLevel() const
{ 
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->ConsoleCommand(TEXT(DISCONNECT_COMMAND), true);
	playerController->ConsoleCommand(TEXT(OPEN_LOBBY_COMMAND), true);
}

void APNGGameModeMain::ServerRPCSpawnBall_Implementation()
{
	mBall = GetWorld()->SpawnActor<APNGBall>(BallType, DEFAULT_BALL_LOCATION, FVector::RightVector.Rotation());
	mBall->SetOwner(GetOwner());

	mBall->OnBallHitGoal().RemoveAll(this);
	mBall->OnBallHitGoal().AddUObject(this, &APNGGameModeMain::OnBallHitGoalHandler);
}

bool APNGGameModeMain::ServerRPCSpawnBall_Validate()
{
	return true;
}
