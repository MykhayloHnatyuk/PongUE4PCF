// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGlobalDefines.h"
#include "Settings/PNGGameStateMain.h"

float GetFixedServerWorldTimeSeconds(UWorld* Wold)
{
	float result = 0.0f;

	if(Wold->IsValidLowLevel())
	{
		const APNGGameStateMain* gs = Cast<APNGGameStateMain>(Wold->GetGameState());
		result = gs->GetFixedServerWorldTimeSeconds();
	}	

	return result;
}