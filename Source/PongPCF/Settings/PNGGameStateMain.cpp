// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGameStateMain.h"
#include "GameFramework/PlayerState.h"
#include "EngineGlobals.h"
#include "Player/PNGPlayerState.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

APNGGameStateMain::APNGGameStateMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
}

void APNGGameStateMain::BeginPlay()
{
	Super::BeginPlay();

	Initialization();
}

void APNGGameStateMain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<FPNGBaseGameState*> GraphToClean;
	mHandlers.GenerateValueArray(GraphToClean);

	for (FPNGBaseGameState* GraphState : GraphToClean)
	{
		delete GraphState;
	}

	Super::EndPlay(EndPlayReason);
}

void APNGGameStateMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasActorBegunPlay())
	{
		return;
	}

	UpdateFixedServerTimeSeconds(DeltaTime);
	ProcessStateMachine();
}

void APNGGameStateMain::Initialization()
{
	if(!GetWorld()->IsServer())
	{
		return;
	}

	mHandlers.Add(PNGGameState::gsNoState, new FPNGGSNoSate());
	mHandlers.Add(PNGGameState::gsWaitingForPlayers, new FPNGGSWaitingForPlayers());
	mHandlers.Add(PNGGameState::gsSetupPlay, new FPNGGSSetupPlay());
	mHandlers.Add(PNGGameState::gsStartingPlay, new FPNGGSStartingPlay());
	mHandlers.Add(PNGGameState::gsPlaying, new FPNGGSPlaying());
	mHandlers.Add(PNGGameState::gsExiting, new FPNGGSExiting());

	mTransitions.Add(PNGGameState::gsNoState, { PNGGameState::gsWaitingForPlayers });
	mTransitions.Add(PNGGameState::gsWaitingForPlayers, { PNGGameState::gsSetupPlay });
	mTransitions.Add(PNGGameState::gsSetupPlay, { PNGGameState::gsStartingPlay });
	mTransitions.Add(PNGGameState::gsStartingPlay, { PNGGameState::gsPlaying, PNGGameState::gsExiting });
	mTransitions.Add(PNGGameState::gsPlaying, { PNGGameState::gsStartingPlay, PNGGameState::gsExiting });
	mTransitions.Add(PNGGameState::gsExiting, { PNGGameState::gsNoState });

	mCurrentState = PNGGameState::gsNoState;
	mDesireState = PNGGameState::gsNoState;

	FPNGBaseGameState* firstState = mHandlers.FindRef(mCurrentState);
	firstState->StartState(GetWorld());
}

void APNGGameStateMain::ProcessStateMachine()
{
	if (!GetWorld()->IsServer())
	{
		return;
	}

	PNGGameState currentStateType = GetState();
	FPNGBaseGameState* currentState = mHandlers.FindRef(currentStateType);

	currentState->ProcessState(GetWorld());

	if(currentState->IsReadyForExit())
	{
		TArray<PNGGameState> transitions = mTransitions.FindRef(currentStateType);
		for(auto transitionToSate : transitions)
		{
			FPNGBaseGameState* nextState = mHandlers.FindRef(transitionToSate);

			if(nextState->IsReadyForActivation(GetWorld(), this))
			{
				UE_LOG(LogType, Log, TEXT("APNGGameStateMain::ProcessStateMachine FROM %d TO %d"), (uint8)mCurrentState, (uint8)transitionToSate);
				currentState->EndState(GetWorld());
				SetState(transitionToSate);
				nextState->StartState(GetWorld());

				if(mDesireState == transitionToSate)
				{
					mDesireState = PNGGameState::gsNoState;
				}

				// Notify all about state change.
				MulticastRPCNotifyStateChange(mCurrentState);
				break;
			}
		}
	}
}

void APNGGameStateMain::TrySwitchState(PNGGameState value)
{
	mDesireState = value;
}

void APNGGameStateMain::UpdateGameScore(int Player1Score, int Player2Score)
{
	MulticastRPCNotifyScoreChange(Player1Score, Player2Score);
}

void APNGGameStateMain::MulticastRPCNotifyStateChange_Implementation(PNGGameState NewState)
{
	// First, lets update state value for our clients.
	if (!GetWorld()->IsServer())
	{
		SetState(NewState);
	}

	UE_LOG(LogType, Log, TEXT("%d APNGGameStateMain::MulticastRPCNotifyStateChange_Implementation NewState: %d"), GetWorld()->IsServer(), int(NewState));

	// Now lets broadcast it for everybody.
	OnGameStateChanged().Broadcast(NewState);
}

void APNGGameStateMain::MulticastRPCNotifyScoreChange_Implementation(int Player1Score, int Player2Score)
{
	UE_LOG(LogType, Log, TEXT("%d APNGGameStateMain::MulticastRPCNotifyScoreChange_Implementation %d %d"), GetWorld()->IsServer(), Player1Score, Player2Score);
	OnGameScoreChanged().Broadcast(Player1Score, Player2Score);
}

void APNGGameStateMain::UpdateFixedServerTimeSeconds(float DeltaTime)
{
	float realServerTimeSecods = GetServerWorldTimeSeconds();
	mFixedServerTimeSeconds += DeltaTime;

	if(mFixedServerTimeSeconds < realServerTimeSecods)
	{
		mFixedServerTimeSeconds = realServerTimeSecods;
	}
}

void FPNGGSNoSate::StartState(UWorld* World)
{
	Super::StartState(World);

	bSkippedOneFrame = false;
}

void FPNGGSNoSate::ProcessState(UWorld* World)
{
	if(bSkippedOneFrame)
	{
		MarkExit(bSkippedOneFrame);
	}
	else
	{
		bSkippedOneFrame = true;
	}
}

void FPNGGSWaitingForPlayers::ProcessState(UWorld* World)
{
	auto players = World->GetGameState()->PlayerArray;
	
	bool bNumOfPlayersIsEnough = players.Num() == NUM_OF_NEEDED_PLAYERS_ONLINE;
	bool bAllPlayersAreReady = true;
	
	if (bNumOfPlayersIsEnough)
	{
		for (auto p : players)
		{
			auto player = Cast<APNGPlayerState>(p);
			ensure(player);

			if(!player->bIsReady)
			{
				bAllPlayersAreReady = false;
				break;
			}
		}
	}
	
	MarkExit(bNumOfPlayersIsEnough && bAllPlayersAreReady);
}

void FPNGGSSetupPlay::StartState(UWorld* World)
{
	MarkExit(true);
}

bool FPNGGSStartingPlay::IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const
{
	return GameStateActor->GetState() == PNGGameState::gsSetupPlay || GameStateActor->GetDesiredState() == PNGGameState::gsStartingPlay;
}

void FPNGGSStartingPlay::StartState(UWorld* World)
{
	FPNGBaseGameState::StartState(World);

	mStartTimer = 0.0f;
}

#define DELAY_BEFORE_START 1.0f
void FPNGGSStartingPlay::ProcessState(UWorld* World)
{
	mStartTimer += World->GetDeltaSeconds();

	if(mStartTimer > DELAY_BEFORE_START)
	{
		MarkExit(true);
	}
}

void FPNGGSPlaying::StartState(UWorld* World)
{
	// We are ready to exit whenever any other state is needed.
	MarkExit(true);
}

bool FPNGGSExiting::IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const
{
	bool numOfPlayersIsEnough = World->GetGameState()->PlayerArray.Num() == NUM_OF_NEEDED_PLAYERS_ONLINE;
	return !numOfPlayersIsEnough;
}
