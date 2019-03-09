// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGameStateMain.h"

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

	ProcessStateMachine();
}

void APNGGameStateMain::Initialization()
{
	mHandlers.Add(GameStates::gsNoState, new FPNGGSNoSate());
	mHandlers.Add(GameStates::gsWaitingForPlayers, new FPNGGSWaitingForPlayers());
	mHandlers.Add(GameStates::gsStartingPlay, new FPNGGSStartingPlay());
	mHandlers.Add(GameStates::gsPlaying, new FPNGGSPlaying());
	mHandlers.Add(GameStates::gsExiting, new FPNGGSExiting());

	mTransitions.Add(GameStates::gsNoState, { GameStates::gsWaitingForPlayers });
	mTransitions.Add(GameStates::gsWaitingForPlayers, { GameStates::gsStartingPlay });
	mTransitions.Add(GameStates::gsStartingPlay, { GameStates::gsPlaying, GameStates::gsExiting });
	mTransitions.Add(GameStates::gsPlaying, { GameStates::gsStartingPlay, GameStates::gsExiting });
	mTransitions.Add(GameStates::gsExiting, { GameStates::gsNoState });

	mCurrentState = GameStates::gsNoState;
	mDesireState = GameStates::gsWaitingForPlayers;

	FPNGBaseGameState* firstState = mHandlers.FindRef(mCurrentState);
	firstState->StartState(GetWorld());
}

void APNGGameStateMain::ProcessStateMachine()
{
	GameStates currentStateType = GetState();
	FPNGBaseGameState* currentState = mHandlers.FindRef(currentStateType);

	currentState->ProcessState(GetWorld());

	if(currentState->IsReadyForExit())
	{
		TArray<GameStates> transitions = mTransitions.FindRef(currentStateType);
		for(auto transitionToSate : transitions)
		{
			FPNGBaseGameState* nextState = mHandlers.FindRef(transitionToSate);

			if(nextState->IsReadyForActivation(GetWorld(), this))
			{
				UE_LOG(LogType, Log, TEXT("APNGGameStateMain::ProcessStateMachine FROM %d TO %d"), (uint8)mCurrentState, (uint8)transitionToSate);
				currentState->EndState(GetWorld());
				mCurrentState = transitionToSate;
				nextState->StartState(GetWorld());

				if(mDesireState == transitionToSate)
				{
					mDesireState = GameStates::gsNoState;
				}

				OnGameStateChanged.Broadcast(mCurrentState);
				break;
			}
		}
	}
}

void APNGGameStateMain::TrySwitchState(GameStates value)
{
	mDesireState = value;
}

void FPNGGSNoSate::StartState(UWorld* World)
{
	MarkExit(true);
}

void FPNGGSWaitingForPlayers::ProcessState(UWorld* World)
{
	// Check how many players connected.
	// if 2 - go to start playing. 
	MarkExit(true);
}

bool FPNGGSStartingPlay::IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const
{
	return GameStateActor->GetState() == GameStates::gsWaitingForPlayers || GameStateActor->GetDesireState() == GameStates::gsStartingPlay;
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

void FPNGGSPlaying::ProcessState(UWorld* World)
{

}

bool FPNGGSExiting::IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const
{
	// if we have less than 2 players
	return false;
}