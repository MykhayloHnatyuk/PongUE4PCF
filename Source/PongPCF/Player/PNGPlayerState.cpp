// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGPlayerState.h"
#include "Net/UnrealNetwork.h"

void APNGPlayerState::OnRep_IsReady()
{

}

void APNGPlayerState::OnRep_IsPlayerOne()
{

}

void APNGPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APNGPlayerState, bIsReady);
	DOREPLIFETIME(APNGPlayerState, bIsPlayerOne);
}