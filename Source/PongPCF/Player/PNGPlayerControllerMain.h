// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PNGPlayerControllerMain.generated.h"

class APNGPawnMain;
class APNGPlayerState;

UCLASS()
class PONGPCF_API APNGPlayerControllerMain : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime) override;

	// Used to bind keyboard inputs.
	virtual void SetupInputComponent() override;

public:

	// Returns true if we are Player#1.
	bool IsPlayerOne() const;

	// Returns casted player state.
	APNGPlayerState* GetPNGPlayerState() const;

private:

	// Initialize custom player state data. PNGGameState will not proceed if it is not called.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCIntializeStateData();

	// Move to location on server. 
	// No need to check if we can move to this position on a server, 
	// since we have no dynamic geometry that can influence us.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCMoveTo(FVector NewLocation);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCMoveTo(FVector NewLocation);

	// Move pawn to selected location.
	void MoveTo(FVector NewLocation);

	// This where the keyboard input comes to.
	void MoveUpInput(float AxisValue);

	// Last cached input.
	float mLastMoveAxisValue;
};
