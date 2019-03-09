// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PNGGameStateMain.h"
#include "PNGGameModeMain.generated.h"

class APNGBall;

UCLASS()
class PONGPCF_API APNGGameModeMain : public AGameModeBase
{
	GENERATED_BODY()

	APNGGameModeMain(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Pong|DynamicActors")
	TSubclassOf<APNGBall> BallType;

private:

	UFUNCTION()
	void OnGameStateChangedHandler(GameStates NewState);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCSpawnBall();

	UPROPERTY()
	APNGBall* mBall;
};
