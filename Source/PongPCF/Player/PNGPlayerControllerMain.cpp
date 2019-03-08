// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGPlayerControllerMain.h"
#include "PNGPawnMain.h"
#include "Kismet/GameplayStatics.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"

void APNGPlayerControllerMain::BeginPlay()
{
	Super::BeginPlay();

	Possess(GetPawn());

	if (GetWorld()->IsServer())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.02f, FColor::Red, FString::Printf(TEXT(" BeginPlay IS SERVER INITED")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.02f, FColor::Green, FString::Printf(TEXT(" BeginPlay CLIENT INITED")));
	}
}

#define MOVE_THRESHOLD 0.01f
void APNGPlayerControllerMain::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(FMath::Abs(mLastMoveAxisValue) < MOVE_THRESHOLD)
	{
		return;
	}

	if (auto* pawn = GetPawn())
	{
		FVector newLocation = pawn->GetActorLocation();
		newLocation += FVector::ForwardVector * mLastMoveAxisValue * 200.f * DeltaTime;
		MoveTo(newLocation);

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

	if(GetWorld()->IsServer())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.02f, FColor::Red, FString::Printf(TEXT("IS SERVER INITED")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.02f, FColor::Green, FString::Printf(TEXT("CLIENT INITED")));
	}
}

bool APNGPlayerControllerMain::ServerRPCMoveTo_Validate(FVector NewLocation)
{
	return true;
}

void APNGPlayerControllerMain::ServerRPCMoveTo_Implementation(FVector NewLocation)
{
	MoveTo(NewLocation);

	FColor color = (GetWorld()->IsServer()) ? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(-1, 0.02f, color, FString::Printf(TEXT("APNGPlayerControllerMain::ServerRPCMoveTo_Implementation newLocation %s"), *NewLocation.ToString()));
}

void APNGPlayerControllerMain::MulticastRPCMoveTo_Implementation(FVector NewLocation)
{
	MoveTo(NewLocation);

	FColor color = (GetWorld()->IsServer()) ? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(-1, 0.02f, color, FString::Printf(TEXT("APNGPlayerControllerMain::MulticastRPCMoveTo_Implementation newLocation %s"), *NewLocation.ToString()));
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

	FColor color = (GetWorld()->IsServer()) ? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(-1, 0.02f, color, FString::Printf(TEXT("APNGPlayerControllerMain::MoveUp mLastMoveAxisValue: %f"), mLastMoveAxisValue));
}