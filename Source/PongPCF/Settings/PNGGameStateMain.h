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
	PNGGameState GetState() const { return mCurrentState; }
	PNGGameState GetDesiredState() const { return mDesireState; }

	float GetFixedServerWorldTimeSeconds() const { return mFixedServerTimeSeconds; }

	DECLARE_EVENT_OneParam(APNGGameStateMain, FPNGGameStateChangedEvent, PNGGameState)
	FPNGGameStateChangedEvent& OnGameStateChanged() { return GameStateChangedEvent; }

	DECLARE_EVENT_TwoParams(APNGGameStateMain, FPNGGameScoreChangedEvent, int, int)
	FPNGGameScoreChangedEvent& OnGameScoreChanged() { return GameScoreChangedEvent; }

	void UpdateGameScore(int Player1Score, int Player2Score);

private:

	void SetState(PNGGameState Value) { mCurrentState = Value; }

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCNotifyStateChange(PNGGameState NewState);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCNotifyScoreChange(int Player1Score, int Player2Score);

	void UpdateFixedServerTimeSeconds(float DeltaTime);

	TMap<PNGGameState, FPNGBaseGameState*> mHandlers;
	TMap<PNGGameState, TArray<PNGGameState>> mTransitions;

	PNGGameState mCurrentState;
	PNGGameState mDesireState;

	FPNGGameStateChangedEvent GameStateChangedEvent;
	FPNGGameScoreChangedEvent GameScoreChangedEvent;

	float mFixedServerTimeSeconds;
};

USTRUCT()
struct FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual ~FPNGBaseGameState() = default;

	virtual bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const { return true; }
	virtual void StartState(UWorld* World) { MarkExit(false); };
	virtual void ProcessState(UWorld* World) {};
	virtual void EndState(UWorld* World) {};

	bool IsReadyForExit() const { return mIsReadyForExit; }

protected:

	void MarkExit(bool Value) { mIsReadyForExit = Value; }

private:

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
struct FPNGGSExiting : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSExiting() override = default;

	virtual bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const override;
};