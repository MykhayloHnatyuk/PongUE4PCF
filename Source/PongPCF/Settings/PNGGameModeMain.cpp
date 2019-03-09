// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGameModeMain.h"
#include "GameObjects/PNGBall.h"
#include "GameObjects/PNGPlayerStart.h"
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
	gs->OnGameStateChanged.RemoveAll(this);
	gs->OnGameStateChanged.AddDynamic(this, &APNGGameModeMain::OnGameStateChangedHandler);

	ServerRPCSpawnBall();
}

AActor* APNGGameModeMain::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* FoundPlayerStart = nullptr;
	UWorld* World = GetWorld();

	for (TActorIterator<APNGPlayerStart> It(World); It; ++It)
	{
		bool isPlayerOne = Player->IsLocalPlayerController() && World->IsServer();

		APNGPlayerStart* PlayerStart = *It;
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

void APNGGameModeMain::OnGameStateChangedHandler(GameStates NewState)
{
	switch (NewState)
	{
	case GameStates::gsWaitingForPlayers:
		break;
	case GameStates::gsStartingPlay:
		mBall->StopBallAtLocation(DEFAULT_BALL_LOCATION);
		break;
	case GameStates::gsPlaying:
		mBall->PushBallInRandomDirection();
		break;
	case GameStates::gsExiting:
		ExitLevel();
	default:
		break;
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
}

bool APNGGameModeMain::ServerRPCSpawnBall_Validate()
{
	return true;
}
