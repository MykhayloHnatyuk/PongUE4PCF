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

	virtual void SetupInputComponent() override;

public:

	bool IsPlayerOne() const;

	APNGPlayerState* GetPNGPlayerState() const;

private:

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCSetReadyState();

	// Move to location on server. 
	// No need to check if we can move to this position on a server, 
	// since we have no dynamic geometry that can influence us.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCMoveTo(FVector NewLocation);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCMoveTo(FVector NewLocation);

	void MoveTo(FVector NewLocation);

	void MoveUpInput(float AxisValue);

	float mLastMoveAxisValue;
};
