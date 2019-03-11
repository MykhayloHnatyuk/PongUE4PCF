// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PNGGameStateMain.generated.h"

#define NUM_OF_NEEDED_PLAYERS_ONLINE 2

struct FPNGBaseGameState;

UENUM(BlueprintType)
enum class PNGGameState : uint8
{
	gsNoState				UMETA(DisplayName = "NoState"),
	gsWaitingForPlayers		UMETA(DisplayName = "WaitingForPlayers"),
	gsSetupPlay				UMETA(DisplayName = "SetupPlay"),
	gsStartingPlay			UMETA(DisplayName = "StartingPlay"),
	gsPlaying				UMETA(DisplayName = "Playing"),
	gsFinished				UMETA(DisplayName = "Finished"),
	gsExiting				UMETA(DisplayName = "Exiting")
};

UCLASS()
class PONGPCF_API APNGGameStateMain : public AGameStateBase
{
	GENERATED_BODY()

public:

	APNGGameStateMain(const FObjectInitializer& ObjectInitializer);
	virtual ~APNGGameStateMain() = default;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void Initialization();
	void ProcessStateMachine();

public:

	void TrySwitchState(PNGGameState value);
	PNGGameState GetCurrentState() const { return mCurrentState; }
	PNGGameState GetDesiredState() const { return mDesireState; }

	float GetFixedServerWorldTimeSeconds() const { return mFixedServerTimeSeconds; }

	DECLARE_EVENT_OneParam(APNGGameStateMain, FPNGGameStateChangedEvent, PNGGameState)
	FPNGGameStateChangedEvent& OnGameStateChanged() { return GameStateChangedEvent; }

	DECLARE_EVENT_TwoParams(APNGGameStateMain, FPNGGameScoreChangedEvent, int, int)
	FPNGGameScoreChangedEvent& OnGameScoreChanged() { return GameScoreChangedEvent; }

	void UpdateGameScore(int Player1Score, int Player2Score);
	
	// Resets game (score, etc.) params to default.
	void ResetGameState();

private:

	void SetCurrentState(PNGGameState Value) { mCurrentState = Value; }

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCNotifyStateChange(PNGGameState NewState);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCNotifyScoreChange(int Player1Score, int Player2Score);

	// Update local variable of a server time, 
	// to make sure that they stay the same even when time is not replicated.
	void UpdateFixedServerTimeSeconds(float DeltaTime);

	// Enum type to state class object representation.
	TMap<PNGGameState, FPNGBaseGameState*> mHandlers;
	// Available state transitions for current (key) state.
	TMap<PNGGameState, TArray<PNGGameState>> mTransitions;

	// Our current state.
	PNGGameState mCurrentState;
	// State we want to reach to.
	PNGGameState mDesireState;

	FPNGGameStateChangedEvent GameStateChangedEvent;
	FPNGGameScoreChangedEvent GameScoreChangedEvent;

	// Server time after local update.
	float mFixedServerTimeSeconds;
};

// Base state class of a state machine.
USTRUCT()
struct FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual ~FPNGBaseGameState() = default;

	// Returns true if we are ready to enter this state.
	virtual bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const { return true; }
	// Called when we enter this state.
	virtual void StartState(UWorld* World) { MarkExit(false); };
	// Called on tick if this is our current state.
	virtual void ProcessState(UWorld* World) {};
	// Called when we exit this state.
	virtual void EndState(UWorld* World) {};
	
	// Returns true if we are ready to leave this state.
	bool IsReadyForExit() const { return mIsReadyForExit; }

protected:

	// Mark this state as ready for exit (ready to make transition to the next one).
	void MarkExit(bool Value) { mIsReadyForExit = Value; }

private:

	// True if we are ready to transition to the next state.
	bool mIsReadyForExit;
};

USTRUCT()
struct FPNGGSNoSate : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSNoSate() override = default;

	virtual void StartState(UWorld* World) override;

	virtual void ProcessState(UWorld* World) override;

private:

	bool bSkippedOneFrame;
};

USTRUCT()
struct FPNGGSWaitingForPlayers : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSWaitingForPlayers() override = default;

	virtual void ProcessState(UWorld* World) override;
	
	// Timer used to make delay before starting play.
	float mStartTimer;
};

USTRUCT()
struct FPNGGSSetupPlay : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSSetupPlay() override = default;

	virtual void StartState(UWorld* World) override;
};

USTRUCT()
struct FPNGGSStartingPlay : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSStartingPlay() override = default;

	virtual bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const override;

	virtual void StartState(UWorld* World) override;

	virtual void ProcessState(UWorld* World) override;

private:

	// Timer used to make delay before starting play.
	float mStartTimer;
};

USTRUCT()
struct FPNGGSPlaying : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSPlaying() override = default;

	virtual void StartState(UWorld* World) override;
};

USTRUCT()
struct FPNGGSFinished : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSFinished() override = default;

	virtual bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const override;

	virtual void StartState(UWorld* World) override;

	virtual void ProcessState(UWorld* World) override;

	virtual void EndState(UWorld* World) override;

private:
	
	// Time to wait until restart.
	float mRestartTimer;
};

USTRUCT()
struct FPNGGSExiting : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSExiting() override = default;

	virtual bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const override;
};