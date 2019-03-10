// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGameStateMain.h"
#include "GameFramework/PlayerState.h"
#include "EngineGlobals.h"
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
	mHandlers.Add(PNGGameState::gsStartingPlay, new FPNGGSStartingPlay());
	mHandlers.Add(PNGGameState::gsPlaying, new FPNGGSPlaying());
	mHandlers.Add(PNGGameState::gsExiting, new FPNGGSExiting());

	mTransitions.Add(PNGGameState::gsNoState, { PNGGameState::gsWaitingForPlayers });
	mTransitions.Add(PNGGameState::gsWaitingForPlayers, { PNGGameState::gsStartingPlay });
	mTransitions.Add(PNGGameState::gsStartingPlay, { PNGGameState::gsPlaying, PNGGameState::gsExiting });
	mTransitions.Add(PNGGameState::gsPlaying, { PNGGameState::gsStartingPlay, PNGGameState::gsExiting });
	mTransitions.Add(PNGGameState::gsExiting, { PNGGameState::gsNoState });

	mCurrentState = PNGGameState::gsNoState;
	mDesireState = PNGGameState::gsWaitingForPlayers;

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

void APNGGameStateMain::MulticastRPCNotifyStateChange_Implementation(PNGGameState NewState)
{
	// First, lets update state value for our clients.
	if (!GetWorld()->IsServer())
	{
		SetState(NewState);
	}

	// Now lets broadcast it for everybody.
	OnGameStateChanged().Broadcast(NewState);
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
	MarkExit(true);
}

void FPNGGSWaitingForPlayers::ProcessState(UWorld* World)
{
	bool numOfPlayersIsEnough = World->GetGameState()->PlayerArray.Num() == NUM_OF_NEEDED_PLAYERS_ONLINE;
	MarkExit(numOfPlayersIsEnough);
}

bool FPNGGSStartingPlay::IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const
{
	return GameStateActor->GetState() == PNGGameState::gsWaitingForPlayers || GameStateActor->GetDesireState() == PNGGameState::gsStartingPlay;
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
	bool numOfPlayersIsEnough = World->GetGameState()->PlayerArray.Num() == NUM_OF_NEEDED_PLAYERS_ONLINE;
	return !numOfPlayersIsEnough;
}
