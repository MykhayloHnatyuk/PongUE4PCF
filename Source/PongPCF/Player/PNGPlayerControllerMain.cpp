// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGPlayerControllerMain.h"
#include "PNGPawnMain.h"
#include "PNGPlayerState.h"
#include "Engine.h"

void APNGPlayerControllerMain::BeginPlay()
{
	Super::BeginPlay();

	// We do it in order to be sure that all players are ready receive network updates,
	// before we will transition to the next game state.
	if(IsLocalPlayerController())
	{
		ServerRPCSetReadyState();
	}

	GetPNGPlayerState()->bIsPlayerOne = IsPlayerOne();
}

#define MOVE_THRESHOLD 0.01f
void APNGPlayerControllerMain::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(FMath::Abs(mLastMoveAxisValue) < MOVE_THRESHOLD)
	{
		return;
	}

	if (auto* pawn = Cast<APNGPawnMain>(GetPawn()))
	{
		FVector newLocation = pawn->GetActorLocation();
		newLocation += FVector::ForwardVector * mLastMoveAxisValue * pawn->GetMovementSpeed() * DeltaTime;
		pawn->LimitMoveToLocation(newLocation);

		if (HasAuthority())
		{
			MulticastRPCMoveTo(newLocation);
		}
		else
		{
			MoveTo(newLocation);
			ServerRPCMoveTo(newLocation);
		}
	}
}

#define MOVE_UP_AXIS_NAME "MoveUp"
void APNGPlayerControllerMain::SetupInputComponent()
{
	Super::SetupInputComponent(); 

	InputComponent->BindAxis(MOVE_UP_AXIS_NAME, this, &APNGPlayerControllerMain::MoveUpInput);
}

bool APNGPlayerControllerMain::IsPlayerOne() const
{
	return IsLocalPlayerController() && GetWorld() && GetWorld()->IsServer();
}

APNGPlayerState* APNGPlayerControllerMain::GetPNGPlayerState() const
{
	APNGPlayerState* state = GetPlayerState<APNGPlayerState>();
	ensure(state);
	return state;
}

bool APNGPlayerControllerMain::ServerRPCMoveTo_Validate(FVector NewLocation)
{
	return true;
}

void APNGPlayerControllerMain::ServerRPCMoveTo_Implementation(FVector NewLocation)
{
	MoveTo(NewLocation);
}

void APNGPlayerControllerMain::MulticastRPCMoveTo_Implementation(FVector NewLocation)
{
	MoveTo(NewLocation);
}

void APNGPlayerControllerMain::ServerRPCSetReadyState_Implementation()
{
	GetPNGPlayerState()->bIsReady = true;
}

bool APNGPlayerControllerMain::ServerRPCSetReadyState_Validate()
{
	return true;
}

void APNGPlayerControllerMain::MoveTo(FVector NewLocation)
{
	if (auto* pawn = GetPawn())
	{
		pawn->SetActorLocation(NewLocation);
	}
}

void APNGPlayerControllerMain::MoveUpInput(float AxisValue)
{
	mLastMoveAxisValue = AxisValue;
}