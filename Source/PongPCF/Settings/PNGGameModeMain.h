// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PNGGameStateMain.h"
#include "PNGGameModeMain.generated.h"

class APNGBall;
class APNGGoalZone;

#define TARGET_GAME_SCORE_TO_WIN 7

UCLASS()
class PONGPCF_API APNGGameModeMain : public AGameModeBase
{
	GENERATED_BODY()

	APNGGameModeMain(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// Choose proper spawn location for our player.
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Pong|DynamicActors")
	TSubclassOf<APNGBall> BallType;

private:

	UFUNCTION()
	void OnGameStateChangedHandler(PNGGameState NewState);

	UFUNCTION()
	void OnBallHitGoalHandler(APNGGoalZone* GoalZone);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCSpawnBall();

	// Go back to Lobby scene.
	void ExitLevel() const;

	UPROPERTY()
	APNGBall* mBall;
};
