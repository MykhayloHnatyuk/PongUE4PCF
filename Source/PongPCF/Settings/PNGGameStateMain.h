// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PNGGameStateMain.generated.h"

#define NUM_OF_NEEDED_PLAYERS_ONLINE 2

struct FPNGBaseGameState;

UENUM(BlueprintType)
enum class GameStates : uint8
{
	gsNoState				UMETA(DisplayName = "NoState"),
	gsWaitingForPlayers		UMETA(DisplayName = "WaitingForPlayers"),
	gsStartingPlay			UMETA(DisplayName = "StartingPlay"),
	gsPlaying				UMETA(DisplayName = "Playing"),
	gsExiting				UMETA(DisplayName = "Exiting")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPNGOnGameStateChanged, GameStates, NewState);

UCLASS()
class PONGPCF_API APNGGameStateMain : public AGameStateBase
{
	GENERATED_BODY()

public:

	APNGGameStateMain(const FObjectInitializer& ObjectInitializer);
	virtual ~APNGGameStateMain() = default;

	FPNGOnGameStateChanged OnGameStateChanged;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void Initialization();
	void ProcessStateMachine();

public:

	void TrySwitchState(GameStates value);
	GameStates GetState() const { return mCurrentState; }
	GameStates GetDesireState() const { return mDesireState; }

	float GetFixedServerWorldTimeSeconds() const { return mFixedServerTimeSeconds; }

private:

	void UpdateFixedServerTimeSeconds(float DeltaTime);

	TMap<GameStates, FPNGBaseGameState*> mHandlers;
	TMap<GameStates, TArray<GameStates>> mTransitions;

	GameStates mCurrentState;
	GameStates mDesireState;

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
};

USTRUCT()
struct FPNGGSWaitingForPlayers : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSWaitingForPlayers() override = default;

	void ProcessState(UWorld* World) override;
};

USTRUCT()
struct FPNGGSStartingPlay : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSStartingPlay() override = default;

	bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const override;

	void StartState(UWorld* World) override;

	void ProcessState(UWorld* World) override;

private:

	// Timer used to make delay before starting play.
	float mStartTimer;
};

USTRUCT()
struct FPNGGSPlaying : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSPlaying() override = default;

	void StartState(UWorld* World) override;

	void ProcessState(UWorld* World) override;
};

USTRUCT()
struct FPNGGSExiting : public FPNGBaseGameState
{
	GENERATED_USTRUCT_BODY()

	virtual	~FPNGGSExiting() override = default;

	bool IsReadyForActivation(UWorld* World, APNGGameStateMain* GameStateActor) const override;
};